/*
 * com2.hpp
 *
 *  Created on: Mar 27, 2019
 *      Author: rev
 */

#ifndef COM2_HPP_
#define COM2_HPP_

#include "stm32f4xx_usart.h"
#include "hardware.h"

class Com2
{
private:


public:
	Com2();
	~Com2();

	void init(uint32_t baudrate);
	void sendByte(char c);
	void sendString(char *s);
	void wait();
};

#endif /* COM2_HPP_ */
