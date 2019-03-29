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
#include <cstdio>
#include "stm32f4xx.h"
#include "hardware.h"
#include "buffer.hpp"

#include "tlc5955.hpp"
#include "stm32spi5.hpp"
#include <stdio.h>

uint16_t buffer_spi[2304];

int main(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	Com1* com1 = Com1::getInstance();
	/*
	 TLC5955 tlc;

	 tlc.init();


	 //spi1.init();

	 tlc.setRGBPinOrder(0,1,2);

	 tlc.setAllDcData(127);
	 tlc.setMaxCurrent(0,0,0);
	 tlc.setFunctionControlData(true, true, true, true, true);
	 tlc.setBrightnessCurrent(127,127,127);
	 tlc.updateControl();

	 */
	com1->sendString("\nRévolution v1.0\nCommunication : En ligne\n");
	while (1) {
		if (com1->dataAvailable()) {
			com1->write(com1->read());
		}
	}
}
