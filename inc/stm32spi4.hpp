/*
 * stm32spi4.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef STM32SPI4_HPP_
#define STM32SPI4_HPP_

class STM32SPI4 : public ISPI {
public:

	STM32SPI4();
	~STM32SPI4();
	void init();
	void sendByte(uint16_t data);
	uint16_t receiveData();

};



#endif /* STM32SPI4_HPP_ */
