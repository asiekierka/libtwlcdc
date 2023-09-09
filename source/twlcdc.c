/*---------------------------------------------------------------------------------

    libtwlcdc - libnds touchscreen controller on 3DS

    Copyright (C) 2023 asie

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:

    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        documentation would be appreciated but is not required.
    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.
    3.	This notice may not be removed or altered from any source
        distribution.

---------------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "twlcdc.h"
#include "codec_internal.h"

static bool wasInitialized = false;
static twlcdcTouchCalibration touchCalibration;
static s32 xscale, yscale;
static s32 xoffset, yoffset;

// thanks, Sono!
static bool twlcdcSetCtrReadMode(bool enabled) {
    u8 value;
    if (R_FAILED(CDCCHK_ReadRegisters2(0x67, 0x25, &value, 1))) {
        return false;
    }
    if (enabled) value |= (1 << 6); else value &= ~(1 << 6);
    if (R_FAILED(CDCCHK_WriteRegisters2(0x67, 0x25, &value, 1))) {
        return false;
    }
    return true;
}

// inner initialization - reconfiguring the SPI buses
static bool twlcdcInitInner(void) {
    if (!twlcdcSetCtrReadMode(false)) {
        return false;
    }
    cdcTouchInit();
    return true;
}

static void twlcdcExitInner(void) {
    cdcTouchExit();
    twlcdcSetCtrReadMode(true);
}

// APT hook - handles uninitializing/reinitializing
static aptHookCookie aptCookie;
static void aptHookCb(APT_HookType hookType, void *param) {
    if (hookType == APTHOOK_ONSUSPEND || hookType == APTHOOK_ONEXIT) {
        twlcdcExitInner();
    } else if (hookType == APTHOOK_ONRESTORE || hookType == APTHOOK_ONWAKEUP) {
        twlcdcInitInner();
    }
}

// user routines
bool twlcdcInit(void) {
    if (!wasInitialized) {
        if (R_FAILED(CFG_GetConfigInfoBlk4(sizeof(touchCalibration), 0x00040000, &touchCalibration))) {
            return false;
        }
        twlcdcTouchSetCalibration(&touchCalibration);
        if (!twlcdcInitInner()) {
            return false;
        }
        aptHook(&aptCookie, aptHookCb, NULL);
        wasInitialized = true;
    }
    return true;
}

void twlcdcExit(void) {
    if (wasInitialized) {
        aptUnhook(&aptCookie);
        twlcdcExitInner();
        wasInitialized = false;
    }
}

twlcdcTouchCalibration *twlcdcTouchGetCalibration(void) {
    return &touchCalibration;
}

#define TXY_SHIFT 19

void twlcdcTouchSetCalibration(const twlcdcTouchCalibration *in) {
    if (in != &touchCalibration) {
        memcpy(&touchCalibration, in, sizeof(touchCalibration));
    }

    xscale = ((touchCalibration.calX2px - touchCalibration.calX1px) << TXY_SHIFT) / ((touchCalibration.calX2) - (touchCalibration.calX1));
    yscale = ((touchCalibration.calY2px - touchCalibration.calY1px) << TXY_SHIFT) / ((touchCalibration.calY2) - (touchCalibration.calY1));

    xoffset = ((touchCalibration.calX1 + touchCalibration.calX2) * xscale - ((touchCalibration.calX1px + touchCalibration.calX2px) << TXY_SHIFT)) / 2;
    yoffset = ((touchCalibration.calY1 + touchCalibration.calY2) * yscale - ((touchCalibration.calY1px + touchCalibration.calY2px) << TXY_SHIFT)) / 2;
}

bool twlcdcTouchPenDown(void) {
    if (!wasInitialized) {
        return false;
    }

    return cdcTouchPenDown();
}

bool twlcdcTouchRead(twlcdcTouchPosition* pos) {
    if (!wasInitialized) {
        return false;
    }

    if (!cdcTouchRead(pos)) {
        return false;
    }

    s16 px = (pos->rawx * xscale - xoffset + xscale/2) >> TXY_SHIFT;
    s16 py = (pos->rawy * yscale - yoffset + yscale/2) >> TXY_SHIFT;

    if (px < 0) px = 0; else if (px >= GSP_SCREEN_HEIGHT_BOTTOM) px = GSP_SCREEN_HEIGHT_BOTTOM - 1;
    if (py < 0) py = 0; else if (py >= GSP_SCREEN_WIDTH) py = GSP_SCREEN_WIDTH - 1;

    pos->px = px;
    pos->py = py;

    return true;
}
