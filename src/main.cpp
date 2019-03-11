/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include <cstdio>
#include "stm32f4xx.h"
#include "hardware.h"
#include "stm32spi1.hpp"
#include "stm32spi5.hpp"

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5,ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	uint8_t buffer[16];
	char dataToSend[64];
	STM32SPI1 spi1;
	STM32SPI5 spiFlash;
	//spi1.init();
	spiFlash.init();
	while(1)
	{

		spiFlash.getDeviceID(buffer);
		sprintf(dataToSend,"Test: % \r\n",buffer);
		printf(dataToSend);



	}

}
