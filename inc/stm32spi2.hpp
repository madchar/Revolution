/*
 * stm32spi2.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef STM32SPI2_HPP_
#define STM32SPI2_HPP_

#include "stm32f4xx_spi.h"
#include "hardware.h"
#include "ispi.hpp"

class STM32SPI2 : public ISPI {
public:

	STM32SPI2();
	~STM32SPI2();
	void init();
	void sendByte(uint16_t data);
	uint16_t receiveData();
	void assert();
	void deassert();
};



#endif /* STM32SPI2_HPP_ */
