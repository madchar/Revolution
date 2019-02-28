/*
 * stm32spi5.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef STM32SPI5_HPP_
#define STM32SPI5_HPP_

class STM32SPI5 : public ISPI {
public:

	STM32SPI5();
	~STM32SPI5();
	void init();
	void sendByte(uint16_t data);
	uint16_t receiveData();

};



#endif /* STM32SPI5_HPP_ */
