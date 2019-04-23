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

/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

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
bool flagRefresh = false;
bool flagLatch = false;

bool flagDMA_TX_Complete1 = false;
bool flagDMA_TX_Complete2 = false;
bool flagDMA_TX_Complete3 = false;
bool flagDMA_TX_Complete4 = false;

static uint8_t bufferIndex = 0;
static uint8_t screenBuffer1[289];
static uint8_t screenBuffer2[289];
static uint8_t screenBuffer3[289];
static uint8_t screenBuffer4[289];
static uint8_t pixelMapBuffer1[2][289];
static uint8_t pixelMapBuffer2[2][289];
static uint8_t pixelMapBuffer3[2][289];
static uint8_t pixelMapBuffer4[2][289];

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
		DMA2_Stream2->CR &= ~(uint32_t) DMA_SxCR_EN;

		if (flagDMA_TX_Complete1 && flagDMA_TX_Complete2 && flagDMA_TX_Complete3
				&& flagDMA_TX_Complete4) {
			if (flagLatch) {
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

void DMA1_Stream4_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET) {
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
		flagDMA_TX_Complete2 = true;
		DMA1_Stream4->CR &= ~(uint32_t) DMA_SxCR_EN;

		if (flagDMA_TX_Complete1 && flagDMA_TX_Complete2 && flagDMA_TX_Complete3
				&& flagDMA_TX_Complete4) {
			if (flagLatch) {
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

		if (flagDMA_TX_Complete1 && flagDMA_TX_Complete2 && flagDMA_TX_Complete3
				&& flagDMA_TX_Complete4) {
			if (flagLatch) {
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

		if (flagDMA_TX_Complete1 && flagDMA_TX_Complete2 && flagDMA_TX_Complete3
				&& flagDMA_TX_Complete4) {
			if (flagLatch) {
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

void TIM4_IRQHandler(void) {
	if (TIM_GetFlagStatus(TIM4, TIM_IT_Update) != RESET) {
		//Clear flag Interrupt
		TIM4->SR = (uint16_t) ~TIM_IT_Update;

		//Disable DMA
		//			    DMA2_Stream2->CR &= ~(uint32_t)DMA_SxCR_EN;
		//			    DMA1_Stream4->CR &= ~(uint32_t)DMA_SxCR_EN;
		//			    DMA1_Stream5->CR &= ~(uint32_t)DMA_SxCR_EN;
		//			    DMA2_Stream1->CR &= ~(uint32_t)DMA_SxCR_EN;
		//				bufferIndex++;
		//				if (bufferIndex==2) bufferIndex = 0;

		//Change DMA memory address
		//				DMA2_Stream2->M0AR = (uint32_t)&pixelMapBuffer1[bufferIndex][0];
		//				DMA1_Stream4->M0AR = (uint32_t)&pixelMapBuffer2[bufferIndex][0];
		//				DMA1_Stream5->M0AR = (uint32_t)&pixelMapBuffer3[bufferIndex][0];
		//				DMA2_Stream1->M0AR = (uint32_t)&pixelMapBuffer4[bufferIndex][0];

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

		flagRefresh = true;
	}
}

void EXTI2_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

		EXTI_ClearITPendingBit(EXTI_Line0);
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

	/*
	 TLC5955 tlc;

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
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
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
	 dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	 dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	 dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	 DMA_Init(DMA2_Stream2, &dma_spi);

	 //DMA SPI2
	 dma_spi.DMA_Channel = DMA_Channel_0;
	 dma_spi.DMA_Memory0BaseAddr = (uint32_t) &screenBuffer2;
	 dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI2->DR));
	 dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	 dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	 dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	 dma_spi.DMA_Mode = DMA_Mode_Normal;
	 dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	 dma_spi.DMA_BufferSize = sizeof(screenBuffer1);
	 dma_spi.DMA_Priority = DMA_Priority_High;
	 dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	 dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	 dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	 DMA_Init(DMA1_Stream4, &dma_spi);

	 //DMA SPI3
	 dma_spi.DMA_Channel = DMA_Channel_0;
	 dma_spi.DMA_Memory0BaseAddr = (uint32_t) &screenBuffer3;
	 dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI3->DR));
	 dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	 dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	 dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	 dma_spi.DMA_Mode = DMA_Mode_Normal;
	 dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	 dma_spi.DMA_BufferSize = sizeof(screenBuffer1);
	 dma_spi.DMA_Priority = DMA_Priority_High;
	 dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	 dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	 dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	 DMA_Init(DMA1_Stream5, &dma_spi);

	 //DMA SPI4
	 dma_spi.DMA_Channel = DMA_Channel_4;
	 dma_spi.DMA_Memory0BaseAddr = (uint32_t)&screenBuffer4;
	 dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI4->DR));
	 dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	 dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	 dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	 dma_spi.DMA_Mode = DMA_Mode_Normal;
	 dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	 dma_spi.DMA_BufferSize = sizeof(screenBuffer1);
	 dma_spi.DMA_Priority = DMA_Priority_High;
	 dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
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

	 // PB2 is connected to EXTI_Line0
	 EXTI_InitStruct.EXTI_Line = EXTI_Line2;
	 // Enable interrupt
	 EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	 // Interrupt mode
	 EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	 // Triggers on rising and falling edge
	 EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	 // Add to EXTI
	 EXTI_Init(&EXTI_InitStruct);

	 //----------------------------------Nested Vectored Interrupt Controller INIT-----------------------------------------------------

	 NVIC_InitTypeDef NVIC_InitStructure;

	 //Enable the DMA2 Stream2 (SPI1_TX) Interrupt
	 NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
	 NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	 //Enable the DMA1 Stream4 (SPI2_TX) Interrupt
	 NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
	 NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	 //Enable the DMA1 Stream5 (SPI3_TX) Interrupt
	 NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
	 NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	 //Enable the DMA2 Stream1 (SPI4_TX) Interrupt
	 NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
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

	 STM32F4Timer refreshRateTimer(TIM4,5973,0,true);
	 refreshRateTimer.enableITUpdate();
	 refreshRateTimer.startTimer();

	 //STM32F4Timer latchTimer(TIM3,)
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

	 tlc.setAllDcData(127);
	 tlc.setMaxCurrent(0, 0, 0);
	 tlc.setFunctionControlData(true, true, true, true, true);
	 tlc.setBrightnessCurrent(127, 127, 127);
	 tlc.updateControl();


	 uint16_t red = 10000;
	 uint16_t green = 0;
	 uint16_t blue = 0;
	 uint16_t lednum = 0;
	 //	for (uint8_t i = 0; i<192;i++)
	 //	{
	 //		tlc.setAllLedsRGB(red,0,0);
	 //		tlc.updateLeds(screenBuffer1,screenBuffer2,screenBuffer3,screenBuffer4);
	 //		bitShift(screenBuffer1, 3);
	 //		bitShift(screenBuffer2, 3);
	 //		bitShift(screenBuffer3, 3);
	 //		bitShift(screenBuffer4, 3);
	 //
	 //		if (red>=65500) ledtoggle = true;
	 //		if (red<=100) ledtoggle = false;
	 //		if(ledtoggle)
	 //		{
	 //			red-=1000;
	 //			//blue-=100;
	 //		}
	 //		else
	 //		{
	 //			red+=1000;
	 //			//blue+=100;
	 //		}
	 //		for (uint16_t j=0;j<289;j++)
	 //		{
	 //			pixelMapBuffer1[i][j] = screenBuffer1[j];
	 //		}
	 //	}
	 tlc.setAllLedsRGB(10000,0,0);
	 //tlc.setLedRGB(PIXELMAP[lednum],10000,0,0);
	 tlc.updateLeds(screenBuffer1,screenBuffer2,screenBuffer3,screenBuffer4);
	 bitShift(screenBuffer1, 3);
	 bitShift(screenBuffer2, 3);
	 bitShift(screenBuffer3, 3);
	 bitShift(screenBuffer4, 3);



	 //Start DMA2 Stream2
	 SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	 DMA_Cmd(DMA2_Stream2, ENABLE);

	 //Start DMA1 Stream4
	 SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	 DMA_Cmd(DMA1_Stream4, ENABLE);

	 //Start DMA1 Stream5
	 SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
	 DMA_Cmd(DMA1_Stream5, ENABLE);

	 //Start DMA2 Stream1
	 SPI_I2S_DMACmd(SPI4, SPI_I2S_DMAReq_Tx, ENABLE);
	 DMA_Cmd(DMA2_Stream1, ENABLE);
	 */
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

