/*
 * com1.cpp
 *
 *  Created on: Mar 27, 2019
 *      Author: rev
 */
#include <com1.hpp>

Com1* Com1::instance = 0;

Com1::Com1() {

	isTransmitting = false;

	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable clock for GPIOA

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// Initialize pins as alternating function
	GPIO_InitStruct.GPIO_Pin = USART1_TX_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART1_TX_GPIO, &GPIO_InitStruct);
	GPIO_PinAFConfig(USART1_TX_GPIO, USART1_TX_PinSource,
			USART1_ALTERNATE_FUNCTION);

	// Initialize pins as alternating function
	GPIO_InitStruct.GPIO_Pin = USART1_RX_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART1_RX_GPIO, &GPIO_InitStruct);
	GPIO_PinAFConfig(USART1_RX_GPIO, USART1_RX_PinSource,
			USART1_ALTERNATE_FUNCTION);

	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	//Enable clock for USART1 peripheral

	/**
	 * Set Baudrate to value you pass to function
	 * Disable Hardware Flow control
	 * Set Mode To TX and RX, so USART will work in full-duplex mode
	 * Disable parity bit
	 * Set 1 stop bit
	 * Set Data bits to 8
	 *
	 * Initialize USART1
	 * Activate USART1
	 */
	USART_InitStruct.USART_BaudRate = 921600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	USART_Cmd(USART1, ENABLE);

	/**
	 * Enable RX interrupt
	 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
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
}
Com1::~Com1() {
	if (instance)
		delete instance;
}
Com1* Com1::getInstance() {
	if (instance == 0)
		instance = new Com1();
	return instance;
}
void Com1::setBaudeRate(uint32_t baudrate) {
	USART1->BRR = (SystemCoreClock >> 2) / baudrate;
}

bool Com1::dataAvailable() {
	return !rxBuffer.isEmpty();
}

uint8_t Com1::read() {
	return rxBuffer.rem();
}

void Com1::write(uint8_t data) {
	txBuffer.add(data);
	if (!isTransmitting) {
		isTransmitting = true;
		USART1->CR1 |= USART_CR1_TXEIE; // active l'interruption.
	}
}
void Com1::sendBytes(uint8_t* data, uint32_t nBytes) {
	for (uint32_t i = 0; i < nBytes; i++) {
		write(*data++);
	}
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

void Com1::incommingDataDecoder(Flash* flash) {
	if (1);
}


extern "C" {
void USART1_IRQHandler(void) {
	volatile unsigned int isr;
	isr = USART1->SR;
	// RX Data
	if (isr & USART_SR_RXNE) {
		USART1->SR &= ~USART_SR_RXNE;
		Com1::instance->rxBuffer.add(USART1->DR);
	}
	// TX Done
	if ((isr & USART_SR_TXE)) {
		USART1->SR &= ~USART_SR_TXE;
		if (Com1::instance->txBuffer.isEmpty()) {
			Com1::instance->isTransmitting = false;
			USART1->CR1 &= (~USART_CR1_TXEIE);
		} else {
			USART1->DR = Com1::instance->txBuffer.rem();
			Com1::instance->isTransmitting = true;
		}
	}
}
}

