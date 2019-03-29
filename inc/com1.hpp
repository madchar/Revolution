/*
 * com.hpp
 *
 *  Created on: Mar 27, 2019
 *      Author: rev
 */

#ifndef COM1_HPP_
#define COM1_HPP_

#include "stm32f4xx_usart.h"
#include "hardware.h"
#include "buffer.hpp"


extern "C" {
void USART1_IRQHandler(void);
}

class Com1
{
public:

	static Com1* getInstance();
	virtual ~Com1();
	void write(uint8_t data);
	void sendString(char *s);
	uint8_t read();
	void getString(char *s);
	bool dataAvailable();
	void setBaudeRate(uint32_t baudrate);


private:

	Com1();

	bool 					isTransmitting;
	Buffer<uint8_t, 32> 	rxBuffer;
	Buffer<uint8_t, 256> 	txBuffer;
	static Com1*			instance;
	friend void USART1_IRQHandler(void);


};


#endif /* COM1_HPP_ */
