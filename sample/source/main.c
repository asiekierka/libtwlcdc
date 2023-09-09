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
#include <citro2d.h>
#include "twlcdc.h"

int main(int argc, char **argv) {
    aptHookCookie aptHookCookie;
    bool initOk = false;

    // Initialize 2D/3D graphics
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // Initialize console on top screen, accelerated render target on bottom screen
    consoleInit(GFX_TOP, NULL);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    printf("libtwlcdc sample\n");

    // Required for twlcdcInit()
    cfguInit();
    cdcChkInit();

    if (twlcdcInit()) {
        initOk = true;
        printf("libtwlcdc initialized!\n\n");
    } else {
        printf("Could not init libtwlcdc!\n\n");
    }

    // Print calibration data
    twlcdcTouchCalibration *calibr = twlcdcTouchGetCalibration();
    printf("Calibr. U/L: [%04X, %04X] => [%d, %d]\n", calibr->calX1, calibr->calY1, calibr->calX1px, calibr->calY1px);
    printf("Calibr. B/R: [%04X, %04X] => [%d, %d]\n", calibr->calX2, calibr->calY2, calibr->calX2px, calibr->calY2px);

    // Main loop
    while (aptMainLoop()) {
        gspWaitForVBlank();
        gfxScreenSwapBuffers(GFX_TOP, false);

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(bottom);

        // Print touch data
        if (initOk && twlcdcTouchPenDown()) {
            twlcdcTouchPosition tpos;
            twlcdcTouchRead(&tpos);

            printf("\x1b[9;1H");
            printf("Raw coords: [%d, %d]            \n", tpos.rawx, tpos.rawy);
            printf("Adj coords: [%d, %d]            \n", tpos.px, tpos.py);
            printf("Touch Z1Z2: [%d, %d]            \n", tpos.z1, tpos.z2);

            if (tpos.z1 != 0) {
                const u32 maxPressure = 0x00DE0000;
                const u32 maxRadius = 128;

                u32 tosend = tpos.rawx * ((tpos.z2 * 4096 / tpos.z1) - 4096);
                float pressure = (float) maxPressure / (float) tosend * maxRadius;

                printf("Resistance: %08lX %lu               \n", tosend, tosend);
                printf("Pressure: %f                  \n", pressure);

                if (!isinf(pressure) && pressure > 0) {
                    u32 circleColor = C2D_Color32(146, 77, 200, 255);
                    u32 maxPixelsPerCircle = 192;
                    float currentPressure = pressure;
                    while (currentPressure > 0) {
                        C2D_DrawCircleSolid(tpos.px, tpos.py, 0.0f, (currentPressure * currentPressure) / maxRadius, circleColor);
                        currentPressure -= maxPixelsPerCircle;
                        // Darken circle color.
                        circleColor = ((circleColor >> 1) & 0x7F7F7F) | 0xFF000000;
                    }
                }
            }
        }

        C3D_FrameEnd(0);

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

    C2D_Fini();

    gfxExit();
    return 0;
}
