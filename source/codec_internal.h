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

#ifndef __LIBTWLCDC_CODEC_INTERNAL_H__
#define __LIBTWLCDC_CODEC_INTERNAL_H__

#include <3ds.h>
#include "twlcdc.h"

#define REG_SPICNT (*(volatile uint16_t*) 0x1EC60000)
#define REG_SPIDATA (*(volatile uint16_t*) 0x1EC60002)

#define SPI_ENABLE  BIT(15)
#define SPI_IRQ     BIT(14)
#define SPI_BUSY    BIT(7)

#define SPI_BAUD_4MHz    0
#define SPI_BAUD_2MHz    1
#define SPI_BAUD_1MHz    2
#define SPI_BAUD_512KHz  3

// Pick the SPI transfer length
#define SPI_BYTE_MODE   (0<<10)
#define SPI_HWORD_MODE  (1<<10)

// Pick the SPI device
#define SPI_DEVICE_POWER      (0 << 8)
#define SPI_DEVICE_FIRMWARE   (1 << 8)
#define SPI_DEVICE_NVRAM      (1 << 8)
#define SPI_DEVICE_TOUCH      (2 << 8)
#define SPI_DEVICE_MICROPHONE (2 << 8)

// When used, the /CS line will stay low after the transfer ends
// i.e. when we're part of a continuous transfer
#define SPI_CONTINUOUS       BIT(11)

enum cdcBanks {
	CDC_CONTROL     = 0x00, // Chip control
	CDC_SOUND       = 0x01, // ADC/DAC control
	CDC_TOUCHCNT	= 0x03, // TSC control
	CDC_TOUCHDATA	= 0xFC, // TSC data buffer
};

void cdcTouchInit(void);
void cdcTouchExit(void);
bool cdcTouchPenDown(void);
bool cdcTouchRead(twlcdcTouchPosition* pos);

#endif /* __LIBTWLCDC_CODEC_INTERNAL_H__ */
