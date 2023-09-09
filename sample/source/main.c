/**
 * libtwlcdc sample program
 *
 * To the extent possible under law, the person who associated CC0 with
 * libtwlcdc sample program has waived all copyright and related or neighboring rights
 * to libtwlcdc sample program.
 *
 * You should have received a copy of the CC0 legalcode along with this
 * work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
 
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include "twlcdc.h"

int main(int argc, char **argv) {
    aptHookCookie aptHookCookie;
    bool initOk = false;

    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    iprintf("libtwlcdc sample\n");

    // Required for twlcdcInit()
    cfguInit();
    cdcChkInit();

    if (twlcdcInit()) {
        initOk = true;
        iprintf("libtwlcdc initialized!\n\n");
    } else {
        iprintf("Could not init libtwlcdc!\n\n");
    }

    // Print calibration data
    twlcdcTouchCalibration *calibr = twlcdcTouchGetCalibration();
    iprintf("Calibr. U/L: [%04X, %04X] => [%d, %d]\n", calibr->calX1, calibr->calY1, calibr->calX1px, calibr->calY1px);
    iprintf("Calibr. B/R: [%04X, %04X] => [%d, %d]\n", calibr->calX2, calibr->calY2, calibr->calX2px, calibr->calY2px);

    // Main loop
    while (aptMainLoop()) {
        gspWaitForVBlank();
        gfxSwapBuffers();

        // Print touch data
        if (initOk && twlcdcTouchPenDown()) {
            twlcdcTouchPosition tpos;
            twlcdcTouchRead(&tpos);
            
            s32 tarea = tpos.z1 != 0 ? (tpos.px * tpos.z2 / tpos.z1) - tpos.px : -1;

            iprintf("\x1b[9;1H");
            iprintf("Raw coords: [%d, %d]            \n", tpos.rawx, tpos.rawy);
            iprintf("Adj coords: [%d, %d]            \n", tpos.px, tpos.py);
            iprintf("Touch Z1Z2: [%d, %d]            \n", tpos.z1, tpos.z2);
            iprintf("Touch area: %ld           \n", tarea);
        }

        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) {
            break;
        }
    }

    if (initOk) {
        aptUnhook(&aptHookCookie);
        twlcdcExit();
    }
    cdcChkExit();
    cfguExit();

    gfxExit();
    return 0;
}
