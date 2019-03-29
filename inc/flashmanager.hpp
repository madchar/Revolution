/*
 * flashmanager.hpp
 *
 *  Created on: Mar 7, 2019
 *      Author: rev
 */

#ifndef FLASHMANAGER_HPP_
#define FLASHMANAGER_HPP_

#include "stm32spi5.hpp"

class FlashManager
{
public:
	STM32SPI5 *spi;
	uint16_t buffer[10];

	FlashManager(ISPI *spi);
	~FlashManager();

	void formatData();
	void writePage(uint32_t address,uint32_t nPage, uint16_t data);
	uint16_t readPage(uint32_t address,uint32_t nPage);
	void writeBlock(uint8_t blockNum, uint16_t data);
	uint16_t readBlock(uint8_t blockNum);
	uint16_t getDeviceID();

private:
	static constexpr uint8_t PageRead 		= 0xD2;
	static constexpr uint8_t WritePage 	= 0x82;
	static constexpr uint8_t DeviceID		= 0x9F;
};



#endif /* FLASHMANAGER_HPP_ */
