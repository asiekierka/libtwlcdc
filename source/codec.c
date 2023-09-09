/*---------------------------------------------------------------------------------

    libtwlcdc - libnds touchscreen controller on 3DS

    Copyright (C) 2023 asie

    Altered code, originally from
    DSi "codec" Touchscreen/Sound Controller control for ARM7

    Copyright (C) 2017
        fincs

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

#include <3ds.h>
#include "twlcdc.h"
#include "codec_internal.h"

//---------------------------------------------------------------------------------
static inline u8 cdcReadReg(u8 bank, u8 reg) {
//---------------------------------------------------------------------------------

    u8 result;
    CDCCHK_ReadRegisters1(bank, reg, &result, 1);
    return result;
}

//---------------------------------------------------------------------------------
static inline void cdcReadRegArray(u8 bank, u8 reg, void* data, u8 size) {
//---------------------------------------------------------------------------------

    CDCCHK_ReadRegisters1(bank, reg, data, size);
}

//---------------------------------------------------------------------------------
static inline void cdcWriteReg(u8 bank, u8 reg, u8 value) {
//---------------------------------------------------------------------------------

    CDCCHK_WriteRegisters1(bank, reg, &value, 1);
}

//---------------------------------------------------------------------------------
static inline void cdcWriteRegMask(u8 bank, u8 reg, u8 mask, u8 value) {
//---------------------------------------------------------------------------------

    cdcWriteReg(bank, reg, (cdcReadReg(bank, reg) & (~mask)) | (value & mask));
}

//---------------------------------------------------------------------------------
static inline void cdcWriteRegArray(u8 bank, u8 reg, const void* data, u8 size) {
//---------------------------------------------------------------------------------

    CDCCHK_WriteRegisters1(bank, reg, data, size);
}

static struct {
    uint8_t reg02;
    uint8_t reg03;
    uint8_t reg04;
    uint8_t reg05;
    uint8_t reg0E;
    uint8_t reg0F;
    uint8_t reg12;
} twlcdcBackup;

//---------------------------------------------------------------------------------
void cdcTouchInit(void) {
//---------------------------------------------------------------------------------

    twlcdcBackup.reg02 = cdcReadReg(CDC_TOUCHCNT, 0x02);
    twlcdcBackup.reg03 = cdcReadReg(CDC_TOUCHCNT, 0x03);
    twlcdcBackup.reg04 = cdcReadReg(CDC_TOUCHCNT, 0x04);
    twlcdcBackup.reg05 = cdcReadReg(CDC_TOUCHCNT, 0x05);
    twlcdcBackup.reg0E = cdcReadReg(CDC_TOUCHCNT, 0x0E);
    twlcdcBackup.reg0F = cdcReadReg(CDC_TOUCHCNT, 0x0F);
    twlcdcBackup.reg12 = cdcReadReg(CDC_TOUCHCNT, 0x12);

    cdcWriteRegMask(CDC_TOUCHCNT, 0x0E, 0x80, 0<<7);
    cdcWriteRegMask(CDC_TOUCHCNT, 0x02, 0x18, 3<<3);
    cdcWriteReg    (CDC_TOUCHCNT, 0x0F, 0xA0);
    cdcWriteRegMask(CDC_TOUCHCNT, 0x0E, 0x38, 5<<3);
    cdcWriteRegMask(CDC_TOUCHCNT, 0x0E, 0x40, 0<<6);
    cdcWriteReg    (CDC_TOUCHCNT, 0x03, 0x8B);
    cdcWriteRegMask(CDC_TOUCHCNT, 0x05, 0x07, 4<<0);
    cdcWriteRegMask(CDC_TOUCHCNT, 0x04, 0x07, 6<<0);
    cdcWriteRegMask(CDC_TOUCHCNT, 0x04, 0x70, 4<<4);
    cdcWriteRegMask(CDC_TOUCHCNT, 0x12, 0x07, 0<<0);
    cdcWriteRegMask(CDC_TOUCHCNT, 0x0E, 0x80, 1<<7);
}

//---------------------------------------------------------------------------------
void cdcTouchExit(void) {
//---------------------------------------------------------------------------------

    cdcWriteReg(CDC_TOUCHCNT, 0x05, twlcdcBackup.reg05);
    cdcWriteReg(CDC_TOUCHCNT, 0x04, twlcdcBackup.reg04);
    cdcWriteReg(CDC_TOUCHCNT, 0x12, twlcdcBackup.reg12);
    cdcWriteReg(CDC_TOUCHCNT, 0x0F, twlcdcBackup.reg0F);
    cdcWriteReg(CDC_TOUCHCNT, 0x03, twlcdcBackup.reg03);
    cdcWriteReg(CDC_TOUCHCNT, 0x02, twlcdcBackup.reg02);
    cdcWriteReg(CDC_TOUCHCNT, 0x0E, twlcdcBackup.reg0E);
}

//---------------------------------------------------------------------------------
bool cdcTouchPenDown(void) {
//---------------------------------------------------------------------------------

    return (cdcReadReg(CDC_TOUCHCNT, 0x09) & 0xC0) != 0x40 && !(cdcReadReg(CDC_TOUCHCNT, 0x0E) & 0x02);
}

//---------------------------------------------------------------------------------
bool cdcTouchRead(twlcdcTouchPosition* pos) {
//---------------------------------------------------------------------------------

    u8 raw[4*2*5];
    u16 arrayX[5], arrayY[5], arrayZ1[5], arrayZ2[5];
    u32 sumX, sumY, sumZ1, sumZ2;
    int i;

    cdcReadRegArray(CDC_TOUCHDATA, 0x01, raw, sizeof(raw));

    for (i = 0; i < 5; i ++) {
        arrayX[i]  = (raw[i*2+ 0]<<8) | raw[i*2+ 1];
        arrayY[i]  = (raw[i*2+10]<<8) | raw[i*2+11];
        arrayZ1[i] = (raw[i*2+20]<<8) | raw[i*2+21];
        arrayZ2[i] = (raw[i*2+30]<<8) | raw[i*2+31];
        if ((arrayX[i] & 0xF000) || (arrayY[i] & 0xF000)) {
            pos->rawx = 0;
            pos->rawy = 0;
            return false;
        }
    }

    // TODO: For now we just average all values without removing inaccurate values
    sumX = 0;
    sumY = 0;
    sumZ1 = 0;
    sumZ2 = 0;
    for (i = 0; i < 5; i ++) {
        sumX += arrayX[i];
        sumY += arrayY[i];
        sumZ1 += arrayZ1[i];
        sumZ2 += arrayZ2[i];
    }

    pos->rawx = sumX / 5;
    pos->rawy = sumY / 5;
    pos->z1 = sumZ1 / 5;
    pos->z2 = sumZ2 / 5;
    return true;
}
