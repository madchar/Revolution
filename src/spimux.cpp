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
void SPIMUX::sendData(uint16_t data)
{
	spi1.sendByte(data);
	spi2.sendByte(data);
	spi3.sendByte(data);
	spi4.sendByte(data);
}
void SPIMUX::sendData(uint16_t data1,uint16_t data2, uint16_t data3, uint16_t data4)
{
	spi1.sendByte(data1);
	spi2.sendByte(data2);
	spi3.sendByte(data3);
	spi4.sendByte(data4);
}

void SPIMUX::setControlDataLatch(bool control_mode_state)
{
	if (control_mode_state)
	{
		spi1.setBitBang();
		spi2.setBitBang();
		spi3.setBitBang();
		spi4.setBitBang();

		spi1.sendControlBits();
		spi2.sendControlBits();
		spi3.sendControlBits();
		spi4.sendControlBits();

		spi1.init();
		spi2.init();
		spi3.init();
		spi4.init();

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

void SPIMUX::assertAll()
{
	spi1.assert();
	spi2.assert();
	spi3.assert();
	spi4.assert();
}

void SPIMUX::setBitBangConfig()
{
	spi1.setBitBang();
	spi2.setBitBang();
	spi3.setBitBang();
	spi4.setBitBang();
}
