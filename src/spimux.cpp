/*
 * spimux.cpp
 *
 *  Created on: Mar 11, 2019
 *      Author: rev
 */

#include "spimux.cpp"
#include "stm32f4xx_gpio.h"
#include "hardware.h"

SPIMUX::SPIMUX()
{

}

SPIMUX::~SPIMUX()
{

}

void SPIMUX::initSPI()
{
	spi1.init();
	spi2.init();
	spi3.init();
	spi4.init();
}

void SPIMUX::sendData(uint16_t *data)
{

}

void SPIMUX::setControlDataLatch(bool control_mode_state)
{
	if (control_mode_state)
	{
		spi1.assert();
		spi2.assert();
		spi3.assert();
		spi4.assert();

	}
	else
	{

	}
}

void SPIMUX::deassertAll()
{
	spi1.deassert();
	spi2.deassert();
	spi3.deassert();
	spi4.deassert();
}

void SPIMUX::setBitBangConfig()
{
	spi1.setBitBang();
	spi2.setBitBang();
	spi3.setBitBang();
	spi4.setBitBang();
}
