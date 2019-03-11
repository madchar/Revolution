/*
 * spimux.cpp
 *
 *  Created on: Mar 11, 2019
 *      Author: rev
 */

#include "spimux.cpp"

SPIMUX::SPIMUX()
{

}

SPIMUX::~SPIMUX()
{

}

void SPIMUX::init()
{
	spi1.init();
	spi2.init();
	spi3.init();
	spi4.init();
}

void SPIMUX::sendData(uint16_t *data)
{

}

void SPIMUX::setControlDataLatch()
{

}
