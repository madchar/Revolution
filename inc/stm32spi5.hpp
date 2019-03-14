/*
 * stm32spi5.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef STM32SPI5_HPP_
#define STM32SPI5_HPP_

#include "stm32f4xx_spi.h"
#include "hardware.h"
#include "ispi.hpp"
#include <stdio.h>


class STM32SPI5 : public ISPI {
public:

	STM32SPI5();
	~STM32SPI5();
	void init();
	void sendByte(uint16_t data);
	void setCS(bool state);
	uint16_t receiveData();
	void assert();
	void deassert();

	void formatData();
	void writePage(uint16_t address,uint16_t nPage, uint16_t *data);
	void readPage(uint32_t address,uint32_t nPage,uint8_t *buffer);
	void writeBlock(uint8_t blockNum, uint16_t data);
	void readBlock(uint8_t blockNum,uint8_t *buffer);
	void getDeviceID(uint8_t *buffer);

private:

	static constexpr uint8_t PageRead 		= 0xD2;

	static constexpr uint8_t Buffer1Write	= 0x84;
	static constexpr uint8_t Buffer2Write   = 0x87;
	static constexpr uint8_t Buffer1ToMem	= 0x83;
	static constexpr uint8_t Buffer2ToMem 	= 0x86;
	static constexpr uint8_t DirectMemWrite	= 0x82;
	static constexpr uint8_t DeviceID		= 0x9F;


};



#endif /* STM32SPI5_HPP_ */
