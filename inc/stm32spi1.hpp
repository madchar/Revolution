/*
 * stm32spi1.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef STM32SPI1_HPP_
#define STM32SPI1_HPP_

#include "stm32f4xx_spi.h"
#include "hardware.h"
#include "ispi.hpp"

class STM32SPI1 : public ISPI {
public:

	STM32SPI1();
	~STM32SPI1();
	void init();
	void sendByte(uint16_t data);
	uint8_t receiveData();
	void assert();
	void deassert();
	void setGPIO(bool spi);

};



#endif /* STM32SPI1_HPP_ */
