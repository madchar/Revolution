/*
 * spimux.hpp
 *
 *  Created on: Mar 1, 2019
 *      Author: rev
 */

#ifndef SPIMUX_HPP_
#define SPIMUX_HPP_

#include "stm32spi1.hpp"
#include "stm32spi2.hpp"
#include "stm32spi3.hpp"
#include "stm32spi4.hpp"

class SPIMUX{

	SPIMUX();
	~SPIMUX();
	void init();
	void sendData(uint16_t *data);
	void setControlDataLatch(); // Datasheet p.20

private:
	STM32SPI1 spi1;
	STM32SPI2 spi2;
	STM32SPI3 spi3;
	STM32SPI4 spi4;

};



#endif /* SPIMUX_HPP_ */
