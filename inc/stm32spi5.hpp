/*
 * stm32spi5.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef STM32SPI5_HPP_
#define STM32SPI5_HPP_

#include "stm32f4xx_spi.h"
#include "hardware.h"
#include "ispi.hpp"

class STM32SPI5 : public ISPI {
public:

	STM32SPI5();
	~STM32SPI5();
	void init();
	void sendByte(uint16_t data);
	uint16_t receiveData();
	void assert();
	void deassert();

};



#endif /* STM32SPI5_HPP_ */
