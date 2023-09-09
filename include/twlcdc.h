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

#ifndef __LIBTWLCDC_H__
#define __LIBTWLCDC_H__

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Touch calibration data structure.
 */
typedef struct twlcdcTouchCalibration {
    s16 calX1; //!< Raw X ADC value, top left
    s16 calY1; //!< Raw Y ADC value, top left
    s16 calX1px; //!< Pixel X value, top left; typically 0 + 32
    s16 calY1px; //!< Pixel Y value, top left; typically 0 + 24
    s16 calX2; //!< Raw X ADC value, bottom right
    s16 calY2; //!< Raw Y ADC value, bottom right
    s16 calX2px; //!< Pixel X value, bottom right; typically 320 - 32
    s16 calY2px; //!< Pixel Y value, bottom right; typically 240 - 24
} twlcdcTouchCalibration;

/**
 * @brief Touch position data structure.
 */
typedef struct twlcdcTouchPosition {
    u16	rawx; //!< Raw X ADC value
    u16	rawy; //!< Raw Y ADC value
    u16	px;   //!< Pixel X value
    u16	py;   //!< Pixel Y value
    u16	z1;   //!< Raw cross-panel resistance (Z1)
    u16	z2;   //!< Raw cross-panel resistance (Z2)
} twlcdcTouchPosition;

/**
 * @brief Initialize libtwlcdc.
 * Initializing cfg:u and cdc:CHK is required beforehand.
 * 
 * @return true Initialization successful.
 * @return false Initialization failed.
 */
bool twlcdcInit(void);

/**
 * @brief Deinitialize libtwlcdc.
 * Must be done when exiting program if twlcdcInit() was called,
 * otherwise the touch screen will stop working until hardware restart.
 */
void twlcdcExit(void);

/**
 * @brief Get the touch calibration data.
 */
twlcdcTouchCalibration *twlcdcTouchGetCalibration(void);

/**
 * @brief Set the touch calibration data.
 *
 * This only affects the library, it is not saved to the system
 * configuration.
 *
 * @param in Touch calibration data to set. The data is copied,
 * it is not accessed via the pointer.
 */
void twlcdcTouchSetCalibration(const twlcdcTouchCalibration *in);

/**
 * @brief Check if the touch screen controller is sensing pen input.
 * 
 * @return true Pen input detected.
 * @return false Pen input not detected.
 */
bool twlcdcTouchPenDown(void);

/**
 * @brief Read touch screen position data.
 * 
 * @param pos Touch screen position data pointer.
 * @return true Data read successfully.
 * @return false Data read failure.
 */
bool twlcdcTouchRead(twlcdcTouchPosition* pos);

#ifdef __cplusplus
}
#endif

#endif /* __LIBTWLCDC_H__ */
