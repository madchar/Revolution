/*
 * hardware.cpp
 *
 *  Created on: Apr 17, 2019
 *      Author: rev
 */
#include "hardware.h"

void initGPIO()
{
	//-----------------------GSCLK INIT---------------------------------------------------------
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
}

void initDMA()
{
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
		dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
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
		dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
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
		dma_spi.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
		dma_spi.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		dma_spi.DMA_FIFOMode = DMA_FIFOMode_Disable;

		DMA_Init(DMA2_Stream1, &dma_spi);

		//Enable the transfer complete interrupt
		DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);
		DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);
		DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);
		DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);

}

void initRCC()
{
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
}

void initEXTI()
{
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
}

void initNVIC()
{
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
//		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);
}

void initTIM()
{


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
}
