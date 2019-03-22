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
#include "tlc5955.hpp"
#include "stm32spi5.hpp"
#include "stm32spi1.hpp"
#include <stdio.h>

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


	//uint8_t buffer[64];
	//char dataToSend[64];
	STM32SPI1 spi1;
	//TLC5955 tlc;
//	printf("TLC created \n\r");
//	tlc.init();
//	tlc.setAllDcData(127);
	//tlc.setMaxCurrent(0,0,0);
//	tlc.setFunctionControlData(true, true, true, true, true);
//	tlc.updateControl();
	spi1.init();
	//tlc.setRGBPinOrder(0,1,2);
	uint8_t data = 0x11;
	//spi1.setBitBang();
	spi1.assert();
	while(1)
	{

		spi1.sendByte8(data);

		//spi1.deassert();
	/*	for(int i = 0;i<10000;i++)
		{
			asm("nop");
		}
		data++;*/
		//spi1.sendControlBits();
		//tlc.setAllLed(50000);
		//tlc.updateLeds();
		//tlc.latch(false);




	}

}
