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
int main(void) {

	Com1* com1 = Com1::getInstance();
	com1->setEcho(false);

	Flash::address_t add;
	add.byte = 0;
	add.page = 99;

	Flash *flash = Flash::getInstance(true);
	//flash->init();
	//flash->readStatusRegisterToString();
	//flash->writeByte(&add,"Hello world!", 12);

	com1->sendString("\n\rRévolution v3.0\n\rCommunication : En ligne\n\r");

	//flash->readByte(&add, bufferSpiRx, 12);

	com1->sendString(bufferSpiRx);

	while (1) {

		com1->incommingDataDecoder(flash);
/*
		 if (com1->dataAvailable()) {
		 com1->write(com1->read());
		 }
*/
	}
}

