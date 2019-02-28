/*
 * stm32spi2.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef STM32SPI2_HPP_
#define STM32SPI2_HPP_

class STM32SPI2 : public ISPI {
public:

	STM32SPI2();
	~STM32SPI2();
	void init();
	void sendByte(uint16_t data);
	uint16_t receiveData();

};



#endif /* STM32SPI2_HPP_ */
