/*
 * stm32spi3.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef STM32SPI3_HPP_
#define STM32SPI3_HPP_

#include "stm32f4xx_spi.h"
#include "hardware.h"
#include "ispi.hpp"

class STM32SPI3 : public ISPI {
public:

	STM32SPI3();
	~STM32SPI3();
	void init();
	void sendByte(uint16_t data);
	uint16_t receiveData();
	void assert();
	void deassert();
};



#endif /* STM32SPI3_HPP_ */
