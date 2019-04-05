
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
#include "tlc5955.hpp"
#include "stm32spi1.hpp"
#include "stm32spi2.hpp"
#include "stm32spi3.hpp"
#include "stm32spi4.hpp"
#include "stm32spi5.hpp"
#include "stm32f4timer.hpp"

static uint16_t screenBuffer1[145];
static uint16_t screenBuffer2[145];
static uint16_t screenBuffer3[145];
static uint16_t screenBuffer4[145];

bool flagLatch1 = false;
bool flagLatch2 = false;
bool flagDMA_TX_Complete = false;

//extern "C" void SysTick_Handler(void);
extern "C" void DMA2_Stream2_IRQHandler(void);



void DMA2_Stream2_IRQHandler(void) {
	if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2) == SET)
	{
		DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
		flagDMA_TX_Complete = true;
	}
}



void bitShift(uint16_t *array, uint8_t chip_count);

/*#define LED2_INIT RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; \
		GPIOA->MODER |= GPIO_MODER_MODER5_0
#define LED2_ON (GPIOA->ODR |= 0x0020)
#define LED2_OFF (GPIOA->ODR &= ~0x0020)
#define LED2_TOGGLE (GPIOA->ODR ^= 0x0020)*/

int main(void) {

	//Stm32f446Timer(TIM_TypeDef * tmr, uint32_t us,bool interruptEnable)
	//	Stm32f446Timer tmr2(TIM2,1000,0);
	//	tmr2.enablePWM(1,10000,255);
	//	tmr2.start();
	//	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	//	GPIOA->MODER |= GPIO_MODER_MODER0_1;
	//	GPIOA->AFR[0] |= 1<<(0*4);
	//
	//	 LED2_INIT;
	//	 while (1)
	//	 {
	//	   LED2_ON;
	//	   LED2_OFF;
	//	 }

	//Stm32f446Timer tmr2(TIM2,1000,0);
	//tmr2.enablePWM(1,10000,255);
	//tmr2.start();

	//----------------------------------RCC INIT-----------------------------------------------------
	//Enable SPIs clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);

	//Enable DMA clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	//Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//Enable Timers clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	//----------------------------------GSCLOCK INIT-----------------------------------------------------

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GSCLK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GSCLK_GPIO, &GPIO_InitStructure);

	GPIO_PinAFConfig(GSCLK_GPIO, GSCLK_PinSource, GPIO_AF_TIM2);

	//----------------------------------DMA INIT-----------------------------------------------------

	DMA_InitTypeDef dma_spi1;

	dma_spi1.DMA_Channel = DMA_Channel_2;
	dma_spi1.DMA_Memory0BaseAddr = (uint32_t) &screenBuffer1;
	dma_spi1.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI1->DR));
	dma_spi1.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi1.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi1.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi1.DMA_Mode = DMA_Mode_Normal;
	dma_spi1.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi1.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi1.DMA_BufferSize = sizeof(screenBuffer1);
	dma_spi1.DMA_Priority = DMA_Priority_High;
	dma_spi1.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi1.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi1.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA2_Stream2, &dma_spi1);

	//Enable the transfer complete interrupt
	DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);


	//----------------------------------Nested Vectored Interrupt Controller INIT-----------------------------------------------------

	NVIC_InitTypeDef NVIC_InitStructure;

	//Enable the DMA2 Stream2 (SPI1_TX) Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	//----------------------------------TIMER INIT-----------------------------------------------------


	STM32F4Timer gsclkTimer(TIM2,1000000,0,0);
	gsclkTimer.enablePWM(1,50);
	gsclkTimer.startTimer();

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
	tlc.setAllLedsRGB(0,0,10000);
	tlc.updateLeds(screenBuffer1);

	bitShift(screenBuffer1, 3);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA2_Stream2, ENABLE);
	while (1) {
//
//		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
//		DMA_Cmd(DMA2_Stream2, ENABLE);
//		while (DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2) == RESET);
//		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
//		tlc.latch(false);
//		tlc.latch(true);

		if (flagDMA_TX_Complete)
		{
			tlc.latch(false);
			tlc.latch(true);
			flagDMA_TX_Complete = false;
			SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
			DMA_Cmd(DMA2_Stream2, ENABLE);
		}


	}

}

void bitShift(uint16_t *array, uint8_t chip_count) {
	uint8_t chips = chip_count;
	uint8_t bitToShift = 16;

	for (uint8_t h = 0; h < chip_count; h++) {
		uint16_t arraySize = (48 * chips);

		uint16_t last = 0;
		uint16_t next = 0;
		for (uint8_t i = 0; i < bitToShift; i++)
		{
			for (uint16_t j = arraySize; j > (arraySize - 48); j--)
			{
				next = (array[j] << 1);
				last = (0x8000 & array[j-1]);
				array[j] = (next | (last >> 15));

			}
		}
		//array[(48*(chips-1))] &= ~(0x0001 << (bitToShift+1));
		chips--;
		bitToShift--;
	}

	for (uint8_t k = 1; k <= chip_count; k++) {
		array[(48 * (k-1))] &= ~(0x0001 << (bitToShift + k));
	}
	array[0] &= (0xFFFF << (bitToShift + 1));

}
