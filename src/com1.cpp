/*
 * com1.cpp
 *
 *  Created on: Mar 27, 2019
 *      Author: rev
 */
#include <com1.hpp>
#include <string.h>
#include <string>
#include <cstdlib>
#include <stm32f4xx_tim.h>

Com1* Com1::instance = 0;

Com1::Com1() {

	commRxCnt = 0;
	pixelColumnBufferCntr = 0;
	memset(pixelColumnBuffer, 0, sizeof(pixelColumnBuffer));
	memset(tram, 0, sizeof(tram));
	echo = false;

	isTransmitting = false;

	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable clock for GPIOA
 

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


	// Initialize pins as alternating function
	GPIO_InitStruct.GPIO_Pin = USART2_TX_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART2_TX_GPIO, &GPIO_InitStruct);
	GPIO_PinAFConfig(USART2_TX_GPIO, USART2_TX_PinSource,USART2_ALTERNATE_FUNCTION);

	// Initialize pins as alternating function
	GPIO_InitStruct.GPIO_Pin = USART2_RX_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART2_RX_GPIO, &GPIO_InitStruct);
	GPIO_PinAFConfig(USART2_RX_GPIO, USART2_RX_PinSource,USART2_ALTERNATE_FUNCTION);

	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	//Enable clock for USART2 peripheral

	/**
	 * Set Channel to USART2
	 * Set Channel Cmd to enable. That will enable USART2 channel in NVIC
	 * Set Both priorities to 0. This means high priority
	 *
	 * Initialize NVIC
	 */
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_EnableIRQ(USART2_IRQn);
	/**
	 * Set Baudrate to value you pass to function
	 * Disable Hardware Flow control
	 * Set Mode To TX and RX, so USART will work in full-duplex mode
	 * Disable parity bit
	 * Set 1 stop bit
	 * Set Data bits to 8
	 *
	 * Initialize USART2
	 * Activate USART2
	 */

	USART_InitStruct.USART_BaudRate = 921600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);

	/**
	 * Enable RX interrupt
	 */

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/**
	 * Set Channel to USART1
	 * Set Channel Cmd to enable. That will enable USART1 channel in NVIC
	 * Set Both priorities to 0. This means high priority
	 *
	 * Initialize NVIC
	 */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_EnableIRQ(USART1_IRQn);

}
Com1::~Com1() {
	if (instance)
		delete instance;
	if (commTimeOut)
		delete commTimeOut;
}
Com1* Com1::getInstance() {
	if (instance == 0)
		instance = new Com1();
	return instance;
}

void Com1::setBaudRate(uint32_t baudrate) {
USART1->BRR = (SystemCoreClock >> 2) / baudrate;

}

bool Com1::dataAvailable() {
	return !rxBuffer.isEmpty();
}

void Com1::setEcho(bool state) {
	echo = state;
}

uint8_t Com1::read() {
	uint8_t tmp;
	USART1->CR1 &= ~USART_CR1_RXNEIE;
	tmp = rxBuffer.rem();
	USART1->CR1 |= USART_CR1_RXNEIE;

	return tmp;
}

void Com1::write(uint8_t data) {
	if (isTransmitting) {
		USART1->CR1 &= ~USART_CR1_TXEIE;
		txBuffer.add(data);
	} else {
		txBuffer.add(data);
		isTransmitting = true;

	}
	USART1->CR1 |= USART_CR1_TXEIE;
}
void Com1::sendBytes(uint8_t* data, uint32_t nBytes) {
	for (uint32_t i = 0; i < nBytes; i++) {
		write(*data++);
	}
}
void Com1::sendByteToString(uint8_t byte) {
	char buffer[33];
	itoa((int) byte, buffer, 10);
	sendString(buffer);
}

void Com1::sendbyteToString(uint16_t byte) {
	char buffer[33];
	itoa(byte, buffer, 10);
	sendString(buffer);
}
void Com1::sendByte8ToBinaryString(uint8_t data) {
	for (int i = 0; i < 8; i++) {
		((data >> (7 - i)) & 0x01) ?
				Com1::getInstance()->write('1') : Com1::getInstance()->write('0');
	}
	Com1::getInstance()->write('\n');
}

void Com1::sendByte16ToBinaryString(uint16_t data) {
	for (int i = 0; i < 16; i++) {
		if ((data >> (15 - i)) & 0x01)
			Com1::getInstance()->write('1');
		else
			Com1::getInstance()->write('0');
	}
	Com1::getInstance()->write('\n');
}

void Com1::sendByte32ToBinaryString(uint32_t data) {
	for (int i = 0; i < 32; i++) {
		if ((data >> (31 - i)) & 0x01)
			Com1::getInstance()->write('1');
		else
			Com1::getInstance()->write('0');
	}
	Com1::getInstance()->write('\n');
}

void Com1::sendString(const char *s) {
	while (*s) {
		write(*s++); // Send Char
	}
}

void Com1::sendString(uint8_t *u) {
	while (*u) {
		write(*u++); // Send Char
	}
}

void Com1::incommingDataDecoder(Flash* flash) {
	uint8_t car;

	switch (commState) {						//Réception d'images

	case IDLE:
		if (dataAvailable()) {
			car = read();
			if (echo)
				write(car);

			switch (parseRxTram) {								//Réception de trames

			case WAIT:
				if (car == '<') {
					parseRxTram = RXPAYLOAD;
					commRxCnt = 0;
				}
				break;

			case RXPAYLOAD:
				tram[commRxCnt++] = car;
				if (commRxCnt >= TRAM_SIZE)
					parseRxTram = VALIDATE;
				break;

			case VALIDATE:
				if (car == '>') {
					parseRxTram = WAIT;
					parseTram(flash);
				} else {
					sendString("TR::ERROR\n\r");
				}
				break;
			}
		}
		break;

	case ASK_FILE_TO_SERVER: {
		std::string s;
		s = "<col" + std::to_string(rxColumnNo) + ">";
		sendString(s.c_str());

		if (retry < MAX_RETRY) {
			commState = SAVE_FILE;
		} else {
			retry = 0;
			commState = TRANSFER_FAILED;
		}
		commState = SAVE_FILE;
	}
		break;

	case SAVE_FILE:
//		commTimeOut->startTimer();
		if (dataAvailable()) {
			pixelColumnBuffer[pixelColumnBufferCntr++] = read();
		}

		if (pixelColumnBufferCntr == Flash::ColumnPixelArraySize) {
			flash->savePixelColumn(rxImageNo, rxColumnNo++, pixelColumnBuffer);
			pixelColumnBufferCntr = 0;
			if (rxColumnNo == Flash::MaxColumnCount) {
				rxColumnNo = 0;
				commState = TRANSFER_COMPLETED;
			} else
				commState = ASK_FILE_TO_SERVER;
		}

		if (timeout) {
			retry++;
			commState = ASK_FILE_TO_SERVER;

			timeoutCntr = 0;
			timeout = false;
//			commTimeOut->stopTimer();
		}
		break;

	case SAVE_FILENAME:
		//commState = IDLE;

		if (dataAvailable()) {
			car = read();
			if (car == '>') {
				filename[commRxCnt] = 0;
				flash->setFilename(rxImageNo, filename);
				commState = IDLE;
			} else
				filename[commRxCnt++] = car;
		}
		break;

	case TRANSFER_COMPLETED:
		flash->setImageInCarrousel(rxImageNo);
		sendString("<txSuccess>");
		memset(filename, 0, sizeof filename);
		commRxCnt = 0;
		commState = SAVE_FILENAME;
		break;

	case TRANSFER_FAILED:
		sendString("Transfer failed!");
		commState = IDLE;
		break;

	}
}

void Com1::parseTram(Flash *flash) {
	std::string sTram = tram;

	if (sTram == CMD_ImageReadyToTransfer) {
		rxImageNo = flash->getNextFreeImageSlot();
		commState = ASK_FILE_TO_SERVER;
	} else if (sTram == CMD_GetFilenameList) {
		sendFilenameList(flash);
	} else if (sTram == CMD_Del0) {
		flash->resetImageInCarrousel(0);
		flash->resetFilename(0);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del1) {
		flash->resetImageInCarrousel(1);
		flash->resetFilename(1);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del2) {
		flash->resetImageInCarrousel(2);
		flash->resetFilename(2);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del3) {
		flash->resetImageInCarrousel(3);
		flash->resetFilename(3);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del4) {
		flash->resetImageInCarrousel(4);
		flash->resetFilename(4);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del5) {
		flash->resetImageInCarrousel(5);
		flash->resetFilename(5);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del6) {
		flash->resetImageInCarrousel(6);
		flash->resetFilename(6);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del7) {
		flash->resetImageInCarrousel(7);
		flash->resetFilename(7);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del8) {
		flash->resetImageInCarrousel(8);
		flash->resetFilename(8);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del9) {
		flash->resetImageInCarrousel(9);
		flash->resetFilename(9);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del10) {
		flash->resetImageInCarrousel(10);
		flash->resetFilename(10);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del11) {
		flash->resetImageInCarrousel(11);
		flash->resetFilename(11);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del12) {
		flash->resetImageInCarrousel(12);
		flash->resetFilename(12);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del13) {
		flash->resetImageInCarrousel(13);
		flash->resetFilename(13);
		sendFilenameList(flash);
	} else if (sTram == CMD_Del14) {
		flash->resetImageInCarrousel(14);
		flash->resetFilename(14);
		sendFilenameList(flash);
	} else {

		std::string s = sTram;
		s.erase(3, 3);
		sTram.erase(0, 3);
		if (s == CMD_Red) {
			flash->redMaxCurrent = stoi(sTram);
			sendString("Red");
			cmdCntr++;
		} else if (s == CMD_Green) {
			flash->greenMaxCurrent = stoi(sTram);
			sendString("Green");
			cmdCntr++;
		} else if (s == CMD_Blue) {
			flash->blueMaxCurrent = stoi(sTram);
			sendString("Blue");
			cmdCntr++;
		} else if (s == CMD_GlobalBrightness) {
			flash->globalBrightness = stoi(sTram);
			sendString("Global");
			cmdCntr++;
		} else if (cmdCntr == 3) {
			flash->writeControlRegister();
			cmdCntr = 0;
		}
	}
}


void Com1::sendFilenameList(Flash* flash) {
	uint16_t cntr = 7;
	char names[40];

	filename[0] = '<';
	filename[1] = 'l';
	filename[2] = 'i';
	filename[3] = 's';
	filename[4] = 't';
	filename[5] = '>';
	filename[6] = ';';
	memset(names, 0, sizeof names);
	for (int i = 0; i < Flash::MaxImageStored; i++) {
		flash->getFilename(i, names);
		uint8_t j = 0;
		while (names[j]) {
			filename[cntr++] = names[j++];
		}
		j = 0;
		filename[cntr++] = ';';
	}
	filename[cntr - 1] = 0;
	sendString(filename);
}




void USART1_IRQHandler(void) {
	volatile unsigned int isr;
	isr = USART2->SR;
// RX Data
	if (isr & USART_SR_RXNE) {
		USART2->SR &= ~USART_SR_RXNE;
		Com1::instance->rxBuffer.add(USART2->DR);
	}
// TX Done
	if ((isr & USART_SR_TXE)) {
		USART2->SR &= ~USART_SR_TXE;
		if (Com1::instance->txBuffer.isEmpty()) {
			Com1::instance->isTransmitting = false;
			USART2->CR1 &= (~USART_CR1_TXEIE);
		} else {
			USART2->DR = Com1::instance->txBuffer.rem();
			Com1::instance->isTransmitting = true;
		}
	}
}

void TIM3_IRQHandler(void) {
	if ((Com1::instance->timeoutCntr++) == Com1::instance->TIME_OUT) {
		Com1::instance->timeout = true;
	}
}
