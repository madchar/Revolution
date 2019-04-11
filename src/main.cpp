
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

bool ledtoggle = false;
bool flagRefresh = false;
bool flagLatch = false;

bool flagDMA_TX_Complete = false;

//extern "C" void SysTick_Handler(void);
extern "C" void DMA2_Stream2_IRQHandler(void);
extern "C" void TIM4_IRQHandler(void);
extern "C" void EXTI2_IRQHandler(void);


void DMA2_Stream2_IRQHandler(void) {
	if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2) == SET)
	{
		DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
		flagDMA_TX_Complete = true;
	}
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetFlagStatus(TIM4,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
		flagRefresh = true;
	}

}

void EXTI2_IRQHandler(void)
{
	  if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

	        EXTI_ClearITPendingBit(EXTI_Line0);
	    }
}


void bitShift(uint8_t *array, uint8_t chip_count);

/*#define LED2_INIT RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; \
		GPIOA->MODER |= GPIO_MODER_MODER5_0
#define LED2_ON (GPIOA->ODR |= 0x0020)
#define LED2_OFF (GPIOA->ODR &= ~0x0020)
#define LED2_TOGGLE (GPIOA->ODR ^= 0x0020)*/

int main(void) {
	static uint8_t screenBuffer1[289];
	static uint8_t screenBuffer2[289];
	static uint8_t screenBuffer3[289];
	static uint8_t screenBuffer4[289];

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
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PULSE_GPIO, &GPIO_InitStructure);

	//----------------------------------DMA INIT-----------------------------------------------------

	DMA_InitTypeDef dma_spi;

	//DMA SPI1
	dma_spi.DMA_Channel = DMA_Channel_2;
	dma_spi.DMA_Memory0BaseAddr = (uint32_t) &screenBuffer1;
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
	dma_spi.DMA_BufferSize = sizeof(screenBuffer2);
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
	dma_spi.DMA_BufferSize = sizeof(screenBuffer3);
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA1_Stream5, &dma_spi);

	//DMA SPI4
	dma_spi.DMA_Channel = DMA_Channel_4;
	dma_spi.DMA_Memory0BaseAddr = (uint32_t) &screenBuffer4;
	dma_spi.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI4->DR));
	dma_spi.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	dma_spi.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_spi.DMA_Mode = DMA_Mode_Normal;
	dma_spi.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_spi.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_spi.DMA_BufferSize = sizeof(screenBuffer4);
	dma_spi.DMA_Priority = DMA_Priority_High;
	dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA2_Stream1, &dma_spi);

	//Enable the transfer complete interrupt
	//DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);

	//----------------------------------External Interrupt INIT-----------------------------------------------------
	  EXTI_InitTypeDef EXTI_InitStruct;

	//PULSE PB2 for EXTI_Line2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);

    /* PB2 is connected to EXTI_Line0 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line2;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

	//----------------------------------Nested Vectored Interrupt Controller INIT-----------------------------------------------------

	NVIC_InitTypeDef NVIC_InitStructure;

	//Enable the DMA2 Stream2 (SPI1_TX) Interrupt
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	//NVIC_EnableIRQ(DMA2_Stream2_IRQn);

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

	STM32F4Timer refreshRateTimer(TIM4,4000,32,true);
	refreshRateTimer.enableITUpdate();
	refreshRateTimer.startTimer();

	STM32F4Timer latchTimer(TIM3)
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

	uint16_t lednum = 0;
	tlc.setLedRGB(lednum,65535,0,0);
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

	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Stream5, ENABLE);

	SPI_I2S_DMACmd(SPI4, SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA2_Stream1, ENABLE);




	while (1) {

		if(flagRefresh)
		{
			tlc.setLedRGB(lednum,0,0,0);
			if(ledtoggle)lednum--;
			else lednum++;
			tlc.setLedRGB(lednum,65535,0,0);
			tlc.updateLeds(screenBuffer1,screenBuffer2,screenBuffer3,screenBuffer4);
			bitShift(screenBuffer1, 3);
			bitShift(screenBuffer2, 3);
			bitShift(screenBuffer3, 3);
			bitShift(screenBuffer4, 3);
			flagRefresh = false;
			if (lednum>=192) ledtoggle = true;
			if (lednum==0) ledtoggle = false;
		}
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
		DMA_Cmd(DMA2_Stream2, ENABLE);

		SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
		DMA_Cmd(DMA1_Stream4, ENABLE);

		SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
		DMA_Cmd(DMA1_Stream5, ENABLE);

		SPI_I2S_DMACmd(SPI4, SPI_I2S_DMAReq_Tx, ENABLE);
		DMA_Cmd(DMA2_Stream1, ENABLE);

		while ((DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2) == RESET)&&(DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4) == RESET)&&(DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5) == RESET)&&(DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_TCIF1) == RESET));
	//	while ((DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5) == RESET)&&(DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_TCIF1) == RESET));
		for(int i = 0;i<100;i++)
		{
			asm("nop");
		}
		tlc.latch(false);
		tlc.latch(true);

		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
		DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);



		//		if (flagDMA_TX_Complete)
		//		{
		//			tlc.latch(false);
		//			tlc.latch(true);
		//			flagDMA_TX_Complete = false;
		//			SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
		//			DMA_Cmd(DMA2_Stream2, ENABLE);
		//			SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
		//			DMA_Cmd(DMA1_Stream4, ENABLE);
		//		}

	}

}

void bitShift(uint8_t *array, uint8_t chip_count) {

//	uint8_t chips = chip_count;
//	uint8_t bitToShift = 7;
//
//	for (uint8_t h = 0; h < chip_count; h++) {
//		uint16_t arraySize = (96 * chips);
//
//		uint8_t last = 0;
//		uint8_t next = 0;
//		for (uint8_t i = 0; i <= bitToShift; i++)
//		{
//			for (uint16_t j = arraySize; j > (arraySize - 96); j--)
//			{
//				next = (array[j] >> 1);
//				last = (0x01 & array[j-1]);
//				array[j] = (next | (last << 7));
//			}
//		}
//		array[(96*(chips-1))] &= ~(0x80 >> (bitToShift));
//		chips--;
//		bitToShift--;
//	}
////	for (uint8_t k = 1; k <= chip_count; k++) {
////		array[(96 * k)] &= ~(0x80 >>(bitToShift + 1));
////	}
//	array[0] &= (0xFF >> (bitToShift-1));



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

	      }
    }
	    if (i == 2) {
	      array[96] &= 0xffffd;
	      for (int j = (288 - 96 - 96); j > ((288 - 96 - 96) - 96); j--) {

	        array[j] &= 0x03;
	        array[j] |= (array[j - 1] << 2);
	        array[j - 1] = array[j - 1] >> 6;

	      }

	    }
	  }
	  array[0] &= 0b00000011;

}
