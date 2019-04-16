/*
 * FlashManager.cpp
 *
 *  Created on: Mar 7, 2019
 *      Author: rev
 */

#include "flashmanager.hpp"

FlashManager::FlashManager(ISPI *spi)
{

}

FlashManager::~FlashManager()
{

}

uint16_t FlashManager::readPage(uint32_t address, uint32_t nPage)
{
	return 0;
}

uint16_t FlashManager::getDeviceID()
{
	uint16_t result = 0;
	spi->assert();
	spi->setCS(false);
	spi->sendByte8(DeviceID);
	for (int i = 0;i<5; i++)
	{
		buffer[i] = spi->receiveData();
	}
	result = spi->receiveData();
	spi->setCS(true);
	spi->deassert();
	return result;
}

void FlashManager::writePage(uint32_t address, uint32_t nPage, uint16_t data)
{

}

void FlashManager::formatData()
{

}
