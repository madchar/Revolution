/**
 ******************************************************************************
 * @file    main.c
 * @author  Matthieu Madran et Alexandre Charrier
 * @version V1.0
 * @date    25-Janvier-2019
 * @brief   Programme principal
 ******************************************************************************
 */

#include "stdint-gcc.h"

#include <cstdio>
#include <stdio.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"

#include "hardware.h"
#include "tlc5955.hpp"
#include "stm32spi1.hpp"
#include "stm32spi2.hpp"
#include "stm32spi3.hpp"
#include "stm32spi4.hpp"
#include "stm32f4timer.hpp"
#include "stm32f411USART1.hpp"
#include "stm32f411USART2.hpp"
#include "flash.hpp"


Flash *flash = Flash::getInstance(false);


uint8_t bufferSpiTx[1156];
uint8_t bufferSpiRx[1156];
uint8_t imageNumber = 0;
uint8_t carrouselCycle = 0;

bool resyncDisplay = false;
bool changeCarrouselImage = false;
bool refreshBuffer = false;
bool DMA_TX_Complete1 = false;
bool DMA_TX_Complete2 = false;
bool DMA_TX_Complete3 = false;
bool DMA_TX_Complete4 = false;

volatile uint16_t pixelColumnCounter = 0;

static bool debug = true;

enum interlace_e{DISPLAY_ODD_SIDE,DISPLAY_EVEN_SIDE,DISPLAY_NONE}interlacing = DISPLAY_ODD_SIDE;
enum display_e{ON,OFF} displayState = ON;
enum bufferIndex_e{BUFFER_1,BUFFER_2} bufferIndex = BUFFER_1;
enum transferState_e {
	IDLE, DISABLE_DISPLAY, TRANSFER_IN_PROGRESS, ENABLE_DISPLAY
} transferState = IDLE;

static uint8_t pixelmapBuffer1[1156];
static uint8_t pixelmapBuffer2[1156];

static constexpr uint8_t blackRow[289] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
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


extern "C" void SysTick_Handler(void);
extern "C" void DMA2_Stream2_IRQHandler(void);
extern "C" void DMA1_Stream4_IRQHandler(void);
extern "C" void DMA1_Stream5_IRQHandler(void);
extern "C" void DMA2_Stream1_IRQHandler(void);
extern "C" void TIM4_IRQHandler(void);
extern "C" void EXTI2_IRQHandler(void);
extern "C" void TIM5_IRQHandler(void);

void DMA2_Stream2_IRQHandler(void) {

	if((DMA2->LISR & DMA_IT_TCIF2) != RESET ) {

		DMA_TX_Complete1 = true;
		DMA2_Stream2->CR &= ~(uint32_t)DMA_SxCR_EN;

		if (DMA_TX_Complete1&&DMA_TX_Complete2&&DMA_TX_Complete3&&DMA_TX_Complete4)
		{
			if(interlacing==DISPLAY_ODD_SIDE)
			{

				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRL = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
			} else {
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRL = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
			}
			DMA_TX_Complete1 = false;
			DMA_TX_Complete2 = false;
			DMA_TX_Complete3 = false;
			DMA_TX_Complete4 = false;
		}
	}
	DMA2->LIFCR = (uint32_t)((DMA_IT_TCIF2|DMA_IT_DMEIF2|DMA_IT_FEIF2|DMA_IT_HTIF2|DMA_IT_TEIF2) & RESERVED_MASK);
	DMA2->LIFCR = (uint32_t)((DMA_FLAG_DMEIF2|DMA_FLAG_FEIF2|DMA_FLAG_HTIF2|DMA_FLAG_TCIF2|DMA_FLAG_TEIF2) & RESERVED_MASK);

}

void DMA1_Stream4_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET) {

		DMA_TX_Complete2 = true;
		DMA1_Stream4->CR &= ~(uint32_t) DMA_SxCR_EN;

		if (DMA_TX_Complete1&&DMA_TX_Complete2&&DMA_TX_Complete3&&DMA_TX_Complete4)
		{
			if(interlacing==DISPLAY_ODD_SIDE)
			{
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRL = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
			} else {
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRL = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
			}

			DMA_TX_Complete1 = false;
			DMA_TX_Complete2 = false;
			DMA_TX_Complete3 = false;
			DMA_TX_Complete4 = false;
		}
	}
	DMA1->HIFCR = (uint32_t)((DMA_IT_TCIF4|DMA_IT_DMEIF4|DMA_IT_FEIF4|DMA_IT_HTIF4|DMA_IT_TEIF4) & RESERVED_MASK);
	DMA1->HIFCR = (uint32_t)((DMA_FLAG_DMEIF4|DMA_FLAG_FEIF4|DMA_FLAG_HTIF4|DMA_FLAG_TCIF4|DMA_FLAG_TEIF4) & RESERVED_MASK);
}

void DMA1_Stream5_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5) == SET) {

		DMA_TX_Complete3 = true;
		DMA1_Stream5->CR &= ~(uint32_t) DMA_SxCR_EN;

		if (DMA_TX_Complete1&&DMA_TX_Complete2&&DMA_TX_Complete3&&DMA_TX_Complete4)
		{
			if(interlacing==DISPLAY_ODD_SIDE)
			{
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRL = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
			} else {
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRL = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
			}
			DMA_TX_Complete1 = false;
			DMA_TX_Complete2 = false;
			DMA_TX_Complete3 = false;
			DMA_TX_Complete4 = false;
		}
	}
	DMA1->HIFCR = (uint32_t)((DMA_IT_TCIF5|DMA_IT_DMEIF5|DMA_IT_FEIF5|DMA_IT_HTIF5|DMA_IT_TEIF5) & RESERVED_MASK);
	DMA1->HIFCR = (uint32_t)((DMA_FLAG_DMEIF5|DMA_FLAG_FEIF5|DMA_FLAG_HTIF5|DMA_FLAG_TCIF5|DMA_FLAG_TEIF5) & RESERVED_MASK);
}

void DMA2_Stream1_IRQHandler(void) {
	if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) == SET) {


		DMA_TX_Complete4 = true;
		DMA2_Stream1->CR &= ~(uint32_t) DMA_SxCR_EN;

		if (DMA_TX_Complete1&&DMA_TX_Complete2&&DMA_TX_Complete3&&DMA_TX_Complete4)
		{
			if(interlacing==DISPLAY_ODD_SIDE)
			{
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRL = TLC_LAT1_Pin;
				TLC_LAT1_GPIO->BSRRH = TLC_LAT1_Pin;
			} else {
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRL = TLC_LAT2_Pin;
				TLC_LAT2_GPIO->BSRRH = TLC_LAT2_Pin;
			}

			DMA_TX_Complete1 = false;
			DMA_TX_Complete2 = false;
			DMA_TX_Complete3 = false;
			DMA_TX_Complete4 = false;
		}
	}
	DMA2->LIFCR = (uint32_t)((DMA_IT_TCIF1|DMA_IT_DMEIF1|DMA_IT_FEIF1|DMA_IT_HTIF1|DMA_IT_TEIF1) & RESERVED_MASK);
	DMA2->LIFCR = (uint32_t)((DMA_FLAG_DMEIF1|DMA_FLAG_FEIF1|DMA_FLAG_HTIF1|DMA_FLAG_TCIF1|DMA_FLAG_TEIF1) & RESERVED_MASK);
}

void TIM4_IRQHandler(void) {
	if ((TIM4->SR & TIM_IT_Update) != RESET) {
		//Clear flag Interrupt
		TIM4->SR = (uint16_t) ~TIM_IT_Update;

		pixelColumnCounter++;
		if(resyncDisplay==true)
		{
			interlacing = DISPLAY_EVEN_SIDE;
			pixelColumnCounter = 62;
			resyncDisplay = false;
		}
		if(interlacing==DISPLAY_EVEN_SIDE)
		{
			if((pixelColumnCounter%2)==0) displayState = OFF;
			else displayState = ON;
		}
		else if(interlacing==DISPLAY_ODD_SIDE)
		{
			if((pixelColumnCounter%2)==0) displayState = ON;
			else displayState = OFF;
		}

		if(pixelColumnCounter==255)displayState = OFF;
		else if(pixelColumnCounter==256)
		{
			if(interlacing==DISPLAY_EVEN_SIDE)
			{
				interlacing = DISPLAY_ODD_SIDE;
				displayState = OFF;
			}
			else if(interlacing==DISPLAY_ODD_SIDE)
			{
				interlacing = DISPLAY_EVEN_SIDE;
				displayState = OFF;
			}
			pixelColumnCounter = 0;
		}

		//Disable DMA
		DMA2_Stream2->CR &= ~(uint32_t) DMA_SxCR_EN;
		DMA1_Stream4->CR &= ~(uint32_t) DMA_SxCR_EN;
		DMA1_Stream5->CR &= ~(uint32_t) DMA_SxCR_EN;
		DMA2_Stream1->CR &= ~(uint32_t) DMA_SxCR_EN;

		switch (displayState){
		case ON:
			switch(bufferIndex){
			case BUFFER_1:
				DMA2_Stream2->M0AR = (uint32_t)&pixelmapBuffer1[867];
				DMA1_Stream4->M0AR = (uint32_t)&pixelmapBuffer1[578];
				DMA1_Stream5->M0AR = (uint32_t)&pixelmapBuffer1[289];
				DMA2_Stream1->M0AR = (uint32_t)&pixelmapBuffer1;
				break;
			case BUFFER_2:
				DMA2_Stream2->M0AR = (uint32_t)&pixelmapBuffer2[867];
				DMA1_Stream4->M0AR = (uint32_t)&pixelmapBuffer2[578];
				DMA1_Stream5->M0AR = (uint32_t)&pixelmapBuffer2[289];
				DMA2_Stream1->M0AR = (uint32_t)&pixelmapBuffer2;
				break;
			}
			if(bufferIndex==BUFFER_2) bufferIndex = BUFFER_1;
			else bufferIndex = BUFFER_2;
			refreshBuffer = true;
			break;
			case OFF:
				DMA2_Stream2->M0AR = (uint32_t)&blackRow;
				DMA1_Stream4->M0AR = (uint32_t)&blackRow;
				DMA1_Stream5->M0AR = (uint32_t)&blackRow;
				DMA2_Stream1->M0AR = (uint32_t)&blackRow;
				break;
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


void TIM5_IRQHandler(void) {
	if ((TIM5->SR & TIM_IT_Update) != RESET) {
		changeCarrouselImage = true;
		TIM5->SR = (uint16_t) ~TIM_IT_Update;
	}
}

void EXTI2_IRQHandler(void)
{
	if ((EXTI->PR & EXTI_Line2) != RESET) {
		resyncDisplay = true;
		changeCarrouselImage = true;
		EXTI->PR = (uint32_t) EXTI_Line2;
	}
}

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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

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
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
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

	//----------------------------------COM---------------------------------------------------------------------
	STM32F411USART1 *wifi = STM32F411USART1::getInstance();
	STM32F411USART2 *console = STM32F411USART2::getInstance();

	//----------------------------------SPI INIT-----------------------------------------------------
	if (debug)
		console->sendString("Initiating SPI...\n\r");
	STM32SPI1 spi1;
	STM32SPI2 spi2;
	STM32SPI3 spi3;
	STM32SPI4 spi4;

	spi1.init();
	spi2.init();
	spi3.init();
	spi4.init();

	if (debug)
		console->sendString("Done.\n\r");

	//----------------------------------TLC5955 INIT-----------------------------------------------------

	if (debug)
		console->sendString("Initiating TLC5955...\n\r");
	TLC5955 tlc;

	tlc.init(&spi1, &spi2, &spi3, &spi4);

	tlc.setRGBPinOrder(0, 1, 2);

	tlc.setAllDcData(127);
	tlc.setMaxCurrent(0, 0, 0);
	tlc.setFunctionControlData(false, true, true, true, true);
	tlc.setBrightnessCurrent(127, 5, 10);
	tlc.updateControl();

	if (debug)
		console->sendString("Done.\n\r");

	//----------------------------------DMA INIT-----------------------------------------------------
	if (debug)
		console->sendString("Initiating DMA...\n\r");
	DMA_InitTypeDef dma_spi;

	//DMA SPI1
	dma_spi.DMA_Channel = DMA_Channel_2;

	dma_spi.DMA_Memory0BaseAddr = (uint32_t)&pixelmapBuffer2;
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI1->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = 289;
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA2_Stream2, &dma_spi);

	//DMA SPI2
	dma_spi.DMA_Channel = DMA_Channel_0;

	dma_spi.DMA_Memory0BaseAddr = (uint32_t)&pixelmapBuffer2[289];
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI2->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = 289;
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA1_Stream4, &dma_spi);

	//DMA SPI3
	dma_spi.DMA_Channel = DMA_Channel_0;

	dma_spi.DMA_Memory0BaseAddr = (uint32_t)&pixelmapBuffer2[578];
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI3->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = 289;
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA1_Stream5, &dma_spi);

	//DMA SPI4
	dma_spi.DMA_Channel = DMA_Channel_4;

	dma_spi.DMA_Memory0BaseAddr = (uint32_t)&pixelmapBuffer2[867];
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI4->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = 289;
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA2_Stream1, &dma_spi);

	//DMA SPI5 RX
	dma_spi.DMA_Channel = DMA_Channel_7;

	dma_spi.DMA_Memory0BaseAddr = (uint32_t)&pixelmapBuffer2[867];
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI5->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = 289;
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA2_Stream5, &dma_spi);

	//DMA SPI5 TX
	dma_spi.DMA_Channel = DMA_Channel_2;

	dma_spi.DMA_Memory0BaseAddr = (uint32_t)&pixelmapBuffer2[867];
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI5->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = 289;
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA2_Stream4, &dma_spi);

	//Enable the transfer complete interrupt
	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

	if (debug)
		console->sendString("Done.\n\r");

	//----------------------------------External Interrupt INIT-----------------------------------------------------
	if (debug)
		console->sendString("Initiating EXTI...\n\r");
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
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);
	if (debug)
		console->sendString("Done.\n\r");

	//----------------------------------Nested Vectored Interrupt Controller INIT-----------------------------------------------------
	if (debug)
		console->sendString("Initiating NVIC...\n\r");
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
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable Timer5 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable EXTI2 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(EXTI2_IRQn);

	if(debug) console->sendString("Done.\n\r");

	//----------------------------------FLASH INIT---------------------------------------------------------
	if(debug) console->sendString("Initiating Flash...\n\r");
	Flash::address_t add;
	add.byte = 0;
	add.page = 99;

	flash->init();
	imageNumber = 0;


	if(debug) console->sendString("Done.\n\r");

	//----------------------------------TIMER INIT-----------------------------------------------------
	if (debug)
		console->sendString("Starting timers...\n\r");
	STM32F4Timer gsclkTimer(TIM2, 1000000, 0, 0);
	gsclkTimer.enablePWM(1, 50);
	gsclkTimer.startTimer();


	//STM32F4Timer latchTimer(TIM4,10,65535,false);
	STM32F4Timer latchTimer(TIM4,5800,0,false); // 5800

	NVIC_EnableIRQ(TIM4_IRQn);
	//Enable timer4 interrupt
	TIM4->DIER |= ENABLE;
	//Start TIMER4
	TIM4->CR1 |= TIM_CR1_CEN;

	if (debug)
		console->sendString("Done.\n\r");



	if(debug) console->sendString("Starting DMA...\n\r");

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

	DMA2_Stream1->CR |= (uint32_t)DMA_SxCR_EN;

	if(debug) console->sendString("Done.\n\r");
	if(debug) console->sendString("All systems nominal.\n\r");

	STM32F411USART1* wifi = STM32F411USART1::getInstance();
	if(debug) console->sendByte16ToBinaryString(flash->getPositionOfPresentImagesInCarrousel());
	while (1) {

		if (changeCarrouselImage)
		{
			carrouselCycle++;
			if(carrouselCycle==55)
			{
				imageNumber++;
				while((flash->positionOfPresentImages&(0x01<<imageNumber))==0)
					{
						imageNumber++;
						if (imageNumber>13) imageNumber = 0;
					}

				if(debug)
				{
					console->sendString("Image number : ");
					console->sendByte8ToBinaryString(imageNumber);
					console->sendString("\n\r");
				}
				if (imageNumber>13) imageNumber = 0;
				carrouselCycle = 0;
			}
			changeCarrouselImage = false;
		}

		wifi->incommingDataDecoder(flash);

		switch (transferState) {
		case IDLE:
			if (wifi->isReadyToTransfer)
				transferState = DISABLE_DISPLAY;
			break;

		case DISABLE_DISPLAY:
			if (debug)
				console->sendString("Disabling display...");
			TIM4->CR1 &= ~TIM_CR1_CEN;
			DMA2_Stream2->CR &= ~(uint32_t) DMA_SxCR_EN;
			DMA1_Stream4->CR &= ~(uint32_t) DMA_SxCR_EN;
			DMA1_Stream5->CR &= ~(uint32_t) DMA_SxCR_EN;
			DMA2_Stream1->CR &= ~(uint32_t) DMA_SxCR_EN;
			refreshBuffer = false;
			EXTI_InitStruct.EXTI_LineCmd = DISABLE;
			EXTI_Init(&EXTI_InitStruct);
			wifi->isOkToTransfer = true;
			transferState = TRANSFER_IN_PROGRESS;
			break;
		case TRANSFER_IN_PROGRESS:
			if (!wifi->isReadyToTransfer) {
				if (debug)
					console->sendString("Transfer Completed...");
				transferState = ENABLE_DISPLAY;
			}
			break;
		case ENABLE_DISPLAY:
			if (debug)
				console->sendString("Enabling display...");
			EXTI_InitStruct.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStruct);
			TIM4->CR1 |= TIM_CR1_CEN;
			wifi->isOkToTransfer = false;
			transferState = IDLE;
			break;
		}

		if(refreshBuffer)
		{
			if(pixelColumnCounter<255)
			{
				if (bufferIndex==BUFFER_1) flash->getPixelColumnDMA(imageNumber,(pixelColumnCounter+1),pixelmapBuffer2);
				else flash->getPixelColumnDMA(imageNumber,(pixelColumnCounter+1),pixelmapBuffer1);
			}
			else if(pixelColumnCounter==255)
			{
				if (bufferIndex==BUFFER_1) flash->getPixelColumnDMA(imageNumber,0,pixelmapBuffer2);
				else flash->getPixelColumnDMA(imageNumber,0,pixelmapBuffer1);
			}
			refreshBuffer = false;

		}
		if(pixelColumnCounter>256) pixelColumnCounter = 0;
	}
}