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
	void initSPI();
	void sendData(uint16_t *data);
	void setControlDataLatch(bool control_mode_state); // Datasheet p.20
	void deassertAll();
	void setBitBangConfig();

private:
	STM32SPI1 spi1;
	STM32SPI2 spi2;
	STM32SPI3 spi3;
	STM32SPI4 spi4;

};



#endif /* SPIMUX_HPP_ */
