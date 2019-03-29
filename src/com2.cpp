/*
 * com1.cpp
 *
 *  Created on: Mar 27, 2019
 *      Author: rev
 */
#include <com2.hpp>

Com2::Com2() {

}
Com2::~Com2() {

}

void Com2::init(uint32_t baudrate) {

	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable clock for GPIOA

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// Initialize pins as alternating function
	GPIO_InitStruct.GPIO_Pin = USART2_TX_Pin | USART2_RX_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART2_TX_GPIO, &GPIO_InitStruct);

	GPIO_PinAFConfig(USART2_TX_GPIO, USART2_TX_PinSource, GPIO_AF_USART2);
	GPIO_PinAFConfig(USART2_RX_GPIO, USART2_RX_PinSource, GPIO_AF_USART2);


	USART_InitTypeDef USART_InitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;

	//Enable clock for USART2 peripheral
/*
	USART_ClockInitTypeDef usart_clockinit;

	usart_clockinit.USART_Clock = USART_Clock_Enable;
	usart_clockinit.USART_CPOL = USART_CPOL_Low;
	usart_clockinit.USART_CPHA = USART_CPHA_1Edge;
	usart_clockinit.USART_LastBit = USART_LastBit_Disable;

	USART_ClockInit(USART2,&usart_clockinit);
*/
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
	USART_InitStruct.USART_BaudRate = baudrate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	USART_Cmd(USART2, ENABLE);

	/**
	 * Enable RX interrupt
	 */
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/**
	 * Set Channel to USART2
	 * Set Channel Cmd to enable. That will enable USART2 channel in NVIC
	 * Set Both priorities to 0. This means high priority
	 *
	 * Initialize NVIC
	 */
	/*NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);*/

}

void Com2::sendByte(char c) {
	//while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); // Wait for Empty
	//USART_SendData(USART2, 'c');
	USART2->DR = ('1' & (uint16_t)0x01FF);
}


void Com2::sendString(char *s) {
	while (*s) {
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); // Wait for Empty
		USART_SendData(USART2, *s++); // Send Char
	}
}

void Com2::wait() {
	for (uint16_t i = 0; i < 200000; i++) {
		asm("nop");
	}
}

