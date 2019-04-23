/**
 ******************************************************************************
 * @file    main.c

 * @author  MadChar
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "stdint-gcc.h"
#include <cstdio>
#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"

#include "hardware.h"
#include "LEDMAP.h"
#include "tlc5955.hpp"
#include "stm32spi1.hpp"
#include "stm32spi2.hpp"
#include "stm32spi3.hpp"
#include "stm32spi4.hpp"
#include "stm32f4timer.hpp"
#include "flash.hpp"

#include <com1.hpp>
#include <flash.hpp>
#include <cstdio>
#include "stm32f4xx.h"
#include "hardware.h"
#include "buffer.hpp"

#include "tlc5955.hpp"
#include <stdio.h>
#include <string.h>



uint8_t bufferSpiTx[1156];

uint8_t bufferSpiRx[1156];


constexpr uint8_t PIXELMAP[] = PIXEL_ARRAY;

bool ledtoggle = true;

bool flagRefreshBuffer = false;
bool flagRefreshRow = false;
bool flagToggleLatch = false;
bool flagDisplayOff = false;
bool flagInterlace = false; // false = odd, true = even

bool flagDMA_TX_Complete1 = false;
bool flagDMA_TX_Complete2 = false;
bool flagDMA_TX_Complete3 = false;
bool flagDMA_TX_Complete4 = false;

static uint8_t blackRow[289] =  {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
};

static uint16_t rowCounter = 0;
static uint8_t bufferIndex = 0;
static uint8_t pixelMapBuffer1[2][289];
static uint8_t pixelMapBuffer2[2][289];
static uint8_t pixelMapBuffer3[2][289];
static uint8_t pixelMapBuffer4[2][289];

static uint8_t screenBuffer1[289];
static uint8_t screenBuffer2[289];
static uint8_t screenBuffer3[289];
static uint8_t screenBuffer4[289];
static uint8_t screenBuffer5[289];
static uint8_t testBuffer1[289] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x70, 0x73, 0xB7,
		0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF,
		0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
		0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
};
static uint8_t testBuffer2[289]  = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF,
		0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFE, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
};
static uint8_t testBuffer3[289]  = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFE, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
};


static uint8_t testBuffer4[289]= {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFC, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
};

//extern "C" void SysTick_Handler(void);
extern "C" void DMA2_Stream2_IRQHandler(void);
extern "C" void DMA1_Stream4_IRQHandler(void);
extern "C" void DMA1_Stream5_IRQHandler(void);
extern "C" void DMA2_Stream1_IRQHandler(void);
extern "C" void TIM4_IRQHandler(void);
extern "C" void EXTI2_IRQHandler(void);

void DMA2_Stream2_IRQHandler(void) {
	if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2) == SET) {
		DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
		flagDMA_TX_Complete1 = true;
		DMA2_Stream2->CR &= ~(uint32_t)DMA_SxCR_EN;

		if (flagDMA_TX_Complete1&&flagDMA_TX_Complete2&&flagDMA_TX_Complete3&&flagDMA_TX_Complete4)
		{
			if(flagInterlace)
			{
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRL = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
			} 
      else 
      {
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRL = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
			}

			flagDMA_TX_Complete1 = false;
			flagDMA_TX_Complete2 = false;
			flagDMA_TX_Complete3 = false;
			flagDMA_TX_Complete4 = false;
		}

	}
}

void DMA1_Stream4_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET) {
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
		flagDMA_TX_Complete2 = true;
		DMA1_Stream4->CR &= ~(uint32_t) DMA_SxCR_EN;


		if (flagDMA_TX_Complete1&&flagDMA_TX_Complete2&&flagDMA_TX_Complete3&&flagDMA_TX_Complete4)
		{
			if(flagInterlace)
			{

				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRL = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
			} else {
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRL = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
			}

			flagDMA_TX_Complete1 = false;
			flagDMA_TX_Complete2 = false;
			flagDMA_TX_Complete3 = false;
			flagDMA_TX_Complete4 = false;
		}
	}
}

void DMA1_Stream5_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5) == SET) {
		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
		DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
		flagDMA_TX_Complete3 = true;
		DMA1_Stream5->CR &= ~(uint32_t) DMA_SxCR_EN;


		if (flagDMA_TX_Complete1&&flagDMA_TX_Complete2&&flagDMA_TX_Complete3&&flagDMA_TX_Complete4)
		{
			if(flagInterlace)
			{

				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRL = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
			} else {
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRL = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
			}

			flagDMA_TX_Complete1 = false;
			flagDMA_TX_Complete2 = false;
			flagDMA_TX_Complete3 = false;
			flagDMA_TX_Complete4 = false;
		}

	}
}

void DMA2_Stream1_IRQHandler(void) {
	if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) == SET) {
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);
		flagDMA_TX_Complete4 = true;
		DMA2_Stream1->CR &= ~(uint32_t) DMA_SxCR_EN;


		if (flagDMA_TX_Complete1&&flagDMA_TX_Complete2&&flagDMA_TX_Complete3&&flagDMA_TX_Complete4)
		{
			if(flagInterlace)
			{
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRL = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
			}
			else
			{


				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRL = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
			}

			flagDMA_TX_Complete1 = false;
			flagDMA_TX_Complete2 = false;
			flagDMA_TX_Complete3 = false;
			flagDMA_TX_Complete4 = false;
		}
	}
}

void TIM4_IRQHandler(void) {
	if (TIM_GetFlagStatus(TIM4, TIM_IT_Update) != RESET) {
		//Clear flag Interrupt
		TIM4->SR = (uint16_t) ~TIM_IT_Update;

		rowCounter++;
		if(rowCounter==255)
		{
			if (flagInterlace)flagInterlace = false;
			else	flagInterlace = true;
			rowCounter = 0;
		}
		//Disable DMA
		DMA2_Stream2->CR &= ~(uint32_t)DMA_SxCR_EN;
		DMA1_Stream4->CR &= ~(uint32_t)DMA_SxCR_EN;
		DMA1_Stream5->CR &= ~(uint32_t)DMA_SxCR_EN;
		DMA2_Stream1->CR &= ~(uint32_t)DMA_SxCR_EN;


		if (flagInterlace)
		{
			if (flagDisplayOff)
			{
				//Change DMA memory address
				DMA2_Stream2->M0AR = (uint32_t)&blackRow;
				DMA1_Stream4->M0AR = (uint32_t)&blackRow;
				DMA1_Stream5->M0AR = (uint32_t)&blackRow;
				DMA2_Stream1->M0AR = (uint32_t)&blackRow;
				flagDisplayOff = false;
			}
			else
			{
				//			bufferIndex++;
				//			if (bufferIndex==2) bufferIndex = 0;
				//Change DMA memory address
				DMA2_Stream2->M0AR = (uint32_t)&testBuffer1;
				DMA1_Stream4->M0AR = (uint32_t)&testBuffer2;
				DMA1_Stream5->M0AR = (uint32_t)&testBuffer3;
				DMA2_Stream1->M0AR = (uint32_t)&testBuffer4;
				flagDisplayOff = true;

				//			DMA2_Stream2->M0AR = (uint32_t)&pixelMapBuffer1[bufferIndex][0];
				//			DMA1_Stream4->M0AR = (uint32_t)&pixelMapBuffer2[bufferIndex][0];
				//			DMA1_Stream5->M0AR = (uint32_t)&pixelMapBuffer3[bufferIndex][0];
				//			DMA2_Stream1->M0AR = (uint32_t)&pixelMapBuffer4[bufferIndex][0];
			}
		}
		else
		{
			if (!flagDisplayOff)
			{
				//Change DMA memory address
				DMA2_Stream2->M0AR = (uint32_t)&blackRow;
				DMA1_Stream4->M0AR = (uint32_t)&blackRow;
				DMA1_Stream5->M0AR = (uint32_t)&blackRow;
				DMA2_Stream1->M0AR = (uint32_t)&blackRow;
				flagDisplayOff = true;
			}
			else
			{
				//			bufferIndex++;
					//			if (bufferIndex==2) bufferIndex = 0;
				//Change DMA memory address
				DMA2_Stream2->M0AR = (uint32_t)&testBuffer1;
				DMA1_Stream4->M0AR = (uint32_t)&testBuffer2;
				DMA1_Stream5->M0AR = (uint32_t)&testBuffer3;
				DMA2_Stream1->M0AR = (uint32_t)&testBuffer4;
				flagDisplayOff = false;
				//			DMA2_Stream2->M0AR = (uint32_t)&pixelMapBuffer1[bufferIndex][0];
				//			DMA1_Stream4->M0AR = (uint32_t)&pixelMapBuffer2[bufferIndex][0];
				//			DMA1_Stream5->M0AR = (uint32_t)&pixelMapBuffer3[bufferIndex][0];
				//			DMA2_Stream1->M0AR = (uint32_t)&pixelMapBuffer4[bufferIndex][0];

			}
		}

		//Start DMA2 Stream2
		SPI1->CR2 |= SPI_I2S_DMAReq_Tx;
		DMA2_Stream2->CR |= (uint32_t) DMA_SxCR_EN;

		//Start DMA1 Stream4
		SPI2->CR2 |= SPI_I2S_DMAReq_Tx;
		DMA1_Stream4->CR |= (uint32_t) DMA_SxCR_EN;

		//Start DMA1 Stream5
		SPI3->CR2 |= SPI_I2S_DMAReq_Tx;
		DMA1_Stream5->CR |= (uint32_t) DMA_SxCR_EN;

		//Start DMA2 Stream1
		SPI4->CR2 |= SPI_I2S_DMAReq_Tx;
		DMA2_Stream1->CR |= (uint32_t) DMA_SxCR_EN;


	}
}

void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) != RESET) {


		//rowCounter = 0;
		flagRefreshBuffer = true;
		TIM4->CR1 &= ~TIM_CR1_CEN;
		TIM4->CR1 |= TIM_CR1_CEN;
		EXTI_ClearITPendingBit(EXTI_Line2);
	}

}

void bitShift(uint8_t *array, uint8_t chip_count);

int main(void) {


	//----------------------------------RCC INIT-----------------------------------------------------
	//Enable SPIs clocks

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);

	//Enable DMA clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	//Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//Enable Timers clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

	//Enable SYSCFG clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//----------------------------------GSCLOCK INIT-----------------------------------------------------

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GSCLK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GSCLK_GPIO, &GPIO_InitStructure);

	GPIO_PinAFConfig(GSCLK_GPIO, GSCLK_PinSource, GPIO_AF_TIM2);

	GPIO_InitStructure.GPIO_Pin = PULSE_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(PULSE_GPIO, &GPIO_InitStructure);

	//---------------LATCH 1 INIT------------------------------------------------------------------------------
	GPIO_InitStructure.GPIO_Pin = TLC_LAT1_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(TLC_LAT1_GPIO, &GPIO_InitStructure);
	//---------------LATCH 2 INIT------------------------------------------------------------------------------
	GPIO_InitStructure.GPIO_Pin = TLC_LAT2_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(TLC_LAT2_GPIO, &GPIO_InitStructure);

	//----------------------------------SPI INIT-----------------------------------------------------

	STM32SPI1 spi1;
	STM32SPI2 spi2;
	STM32SPI3 spi3;
	STM32SPI4 spi4;

	spi1.init();
	spi2.init();
	spi3.init();
	spi4.init();


	//----------------------------------TLC5955 INIT-----------------------------------------------------
	TLC5955 tlc;

	tlc.init(&spi1, &spi2, &spi3, &spi4);

	tlc.setRGBPinOrder(0, 1, 2);

	tlc.setAllDcData(10);
	tlc.setMaxCurrent(0, 0, 0);
	tlc.setFunctionControlData(true, true, true, true, true);
	tlc.setBrightnessCurrent(127, 5, 10);
	tlc.updateControl();

	//----------------------------------DMA INIT-----------------------------------------------------

	DMA_InitTypeDef dma_spi;

	//DMA SPI1
	dma_spi.DMA_Channel = DMA_Channel_2;
	dma_spi.DMA_Memory0BaseAddr = (uint32_t)&screenBuffer1;
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI1->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = sizeof(screenBuffer1);
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA2_Stream2, &dma_spi);

	//DMA SPI2
	dma_spi.DMA_Channel = DMA_Channel_0;
	dma_spi.DMA_Memory0BaseAddr = (uint32_t) &screenBuffer1;
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI2->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = sizeof(screenBuffer1);
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA1_Stream4, &dma_spi);

	//DMA SPI3
	dma_spi.DMA_Channel = DMA_Channel_0;
	dma_spi.DMA_Memory0BaseAddr = (uint32_t) &screenBuffer2;
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI3->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = sizeof(screenBuffer1);
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA1_Stream5, &dma_spi);

	//DMA SPI4
	dma_spi.DMA_Channel = DMA_Channel_4;
	dma_spi.DMA_Memory0BaseAddr = (uint32_t)&screenBuffer2;
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI4->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = sizeof(screenBuffer1);
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA2_Stream1, &dma_spi);

	//Enable the transfer complete interrupt
	DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);

	//----------------------------------External Interrupt INIT-----------------------------------------------------
	EXTI_InitTypeDef EXTI_InitStruct;

	//PULSE PB2 for EXTI_Line2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);

	/* PB2 is connected to EXTI_Line2 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line2;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising and falling edge */
	EXTI_InitStruct.EXTI_Trigger =  EXTI_Trigger_Rising;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

	//----------------------------------Nested Vectored Interrupt Controller INIT-----------------------------------------------------

	NVIC_InitTypeDef NVIC_InitStructure;

	//Enable the DMA2 Stream2 (SPI1_TX) Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	//Enable the DMA1 Stream4 (SPI2_TX) Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	//Enable the DMA1 Stream5 (SPI3_TX) Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	//Enable the DMA2 Stream1 (SPI4_TX) Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(DMA2_Stream1_IRQn);

	// Enable Timer4 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable EXTI2 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//----------------------------------TIMER INIT-----------------------------------------------------


	STM32F4Timer gsclkTimer(TIM2,1000000,0,0);
	gsclkTimer.enablePWM(1,50);
	gsclkTimer.startTimer();

	STM32F4Timer latchTimer(TIM4,5973,0,true);
	latchTimer.enableITUpdate();
	latchTimer.startTimer();
	//	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
	//	TIM_BaseInitStructure.TIM_Period = 16741;
	//	TIM_BaseInitStructure.TIM_Prescaler = 0;
	//	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//	TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
	//	TIM_TimeBaseInit(TIM4, &TIM_BaseInitStructure);
	//
	//	NVIC_EnableIRQ(TIM4_IRQn);
	//	NVIC_SetPriority(TIM4_IRQn,1);
	//	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	//	TIM4->CR1 |= TIM_CR1_CEN;

	//	tlc.setAllLedsRGB(10000,0,0);
	//	tlc.updateLeds(screenBuffer1,screenBuffer5,screenBuffer3,screenBuffer4);
	//	tlc.setAllLedsRGB(0,0,400);
	//	tlc.updateLeds(screenBuffer5,screenBuffer2,screenBuffer3,screenBuffer4);
	//	tlc.setAllLedsRGB(10000,2000,0);
	//	tlc.updateLeds(screenBuffer5,screenBuffer5,screenBuffer3,screenBuffer4);
	//	bitShift(testBuffer1, 3);
	//	bitShift(testBuffer2, 3);
	//	bitShift(testBuffer3, 3);
	//	bitShift(testBuffer4, 3);


	//Start DMA2 Stream2
	SPI1->CR2 |= SPI_I2S_DMAReq_Tx;
	DMA2_Stream2->CR |= (uint32_t)DMA_SxCR_EN;

	//Start DMA1 Stream4
	SPI2->CR2 |= SPI_I2S_DMAReq_Tx;
	DMA1_Stream4->CR |= (uint32_t)DMA_SxCR_EN;

	//Start DMA1 Stream5
	SPI3->CR2 |= SPI_I2S_DMAReq_Tx;
	DMA1_Stream5->CR |= (uint32_t)DMA_SxCR_EN;

	//Start DMA2 Stream1
	SPI4->CR2 |= SPI_I2S_DMAReq_Tx;
	DMA2_Stream1->CR |= (uint32_t)DMA_SxCR_EN;

	while (1) {

//		if(flagRefreshBuffer)
//		{
//			if (bufferIndex==0) Flash::getInstance()->getPixelColumn(0,rowCounter,pixelMapBuffer1[1],pixelMapBuffer2[1],pixelMapBuffer3[1],pixelMapBuffer4[1]);
//			else if(bufferIndex==1) Flash::getInstance()->getPixelColumn(0,rowCounter,pixelMapBuffer1[0],pixelMapBuffer2[0],pixelMapBuffer3[0],pixelMapBuffer4[0]);
//			flagRefreshBuffer = false;
//		}
	}

}

void bitShift(uint8_t *array, uint8_t chip_count) {

	for (uint8_t i = 0; i < chip_count; i++) {
		if (i == 0) {
			for (int j = 288; j > (288 - 96); j--) {
				array[j] = array[j - 1];
			}

		}
		if (i == 1) {
			array[192] = 0;
			for (int j = (288 - 96); j > (288 - 96) - 96; j--) {

				array[j] &= 0x01;
				array[j] |= (array[j - 1] << 1);
				array[j - 1] = array[j - 1] >> 7;

	Com1* com1 = Com1::getInstance();
	com1->setEcho(false);
	Flash::address_t add;
	add.byte = 0;
	add.page = 99;

	Flash *flash = Flash::getInstance(true);
	flash->init();
	//flash->resetImageCount();

	com1->sendString("\nRévolution v3.0\nCommunication : En ligne\n");


	//com1->sendByte8ToBinaryString(flash->getNextFreeImageSlot());
	for (int i = 0; i < 32; i++)
		//com1->sendByte8ToBinaryString(flash->readByte(&add));
		while (1) {
			com1->incommingDataDecoder(flash);

			/*
			if (com1->dataAvailable()) {
				com1->write(com1->read());
			}
			*/
		}
}

