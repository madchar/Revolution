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
#include "com1.hpp"
#include <string.h>

//uint8_t bufferSpiTx[1156];
uint8_t spi1[289];
uint8_t spi2[289];
uint8_t spi3[289];
uint8_t spi4[289];

//uint8_t bufferSpiRx[1156];

char tab[240];

int main(void) {

	memset(tab, 0, sizeof tab);
	tab[0] = '-';
	tab[1] = 'l';
	tab[2] = 'i';
	tab[3] = 's';
	tab[4] = 't';
	tab[5] = '_';
	tab[6] = 't';
	tab[7] = 'e';
	tab[8] = 's';
	tab[9] = 't';

	Com1* com1 = Com1::getInstance();
	com1->setEcho(false);
	com1->sendString("\rRévolution v3.0\rCommunication : En ligne\r");

	Flash *flash = Flash::getInstance(false);
	flash->init();
	Flash::address_t add;
	add.byte = 0;
	add.page = 9;

//	com1->sendByte32ToBinaryString(flash->positionOfPresentImages);
//	com1->sendString("\n\r");

//	flash->getPixelColumn(0, 0, spi1, spi2, spi3, spi4);

	flash->readStatusRegisterToString();
	while (flash->isBusy())
//		;
//	uint8_t byte[] = "1234";
//	flash->writeByte(&add, byte, 4);
	for (int i = 0; i < 15; i++) {
//		flash->setFilename(i, "test.bmp");
//		flash->resetFilename(i);
//		flash->resetImageInCarrousel(i);l
//		com1->write(spi4[i]);
	}

	//flash->resetFilename(1);
	//flash->setFilename(4, "1234567890123456789012345678901234");
	//
	//flash->setFilename(4, "black_hole.bmp");

//	for (int i = 0; i < 240; i++) {
//		com1->write(flash->readByte(&add));
//		add.byte++;
//	}
//	com1->sendString("\n\r");
//	for (int i = 0; i < 289; i++) {
//		com1->write(spi1[i]);
//	}

	while (1) {

		com1->incommingDataDecoder(flash);
		/*
		 if (com1->dataAvailable()) {
		 com1->write(com1->read());
		 }
		 */

//		if(flagRefreshBuffer)
//		{
//			if (bufferIndex==0) Flash::getInstance()->getPixelColumn(0,rowCounter,pixelMapBuffer1[1],pixelMapBuffer2[1],pixelMapBuffer3[1],pixelMapBuffer4[1]);
//			else if(bufferIndex==1) Flash::getInstance()->getPixelColumn(0,rowCounter,pixelMapBuffer1[0],pixelMapBuffer2[0],pixelMapBuffer3[0],pixelMapBuffer4[0]);
//			flagRefreshBuffer = false;
//		}
	}
}

