/*
 * flash.cpp
 *
 *  Created on: Mar 1, 2019
 *      Author: Kloumbi
 */
#include <flash.hpp>
#include "com1.hpp"
#include <string.h>

Flash* Flash::instance = 0;

Flash::Flash() {

}

Flash::~Flash() {
	savePositionOfPresentImagesInCarrousel();
	if (instance)
		delete instance;
}

Flash* Flash::getInstance(bool debug) {
	if (instance == 0)
		instance = new Flash(debug);
	return instance;
}

void Flash::init() {
	/*
	 //------------------------GPIOB------------------------------------------
	 GPIO_InitTypeDef GPIOB_InitStructure;
	 GPIOB_InitStructure.GPIO_Pin = SPI5_CLK_Pin;
	 GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	 GPIOB_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIOB_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	 GPIO_Init(GPIOB, &GPIOB_InitStructure);

	 GPIO_PinAFConfig(SPI5_CLK_GPIO,SPI5_CLK_PinSource,SPI5_ALTERNATE_FUNCTION);

	 GPIO_InitTypeDef GPIO_InitStructure;
	 GPIO_InitStructure.GPIO_Pin = SPI5_NSS_Pin;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	 */

	//------------------------GPIOA MOSI MISO SCLK------------------------------------------
	GPIO_InitTypeDef GPIOA_InitStructure;
	GPIOA_InitStructure.GPIO_Pin = SPI5_MOSI_Pin | SPI5_MISO_Pin | SPI5_CLK_Pin;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOA_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOA_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIOA_InitStructure);

	GPIO_PinAFConfig(SPI5_MOSI_GPIO, SPI5_MOSI_PinSource,
	SPI5_ALTERNATE_FUNCTION);
	GPIO_PinAFConfig(SPI5_MISO_GPIO, SPI5_MISO_PinSource,
	SPI5_ALTERNATE_FUNCTION);
	GPIO_PinAFConfig(SPI5_CLK_GPIO, SPI5_CLK_PinSource,
	SPI5_ALTERNATE_FUNCTION);

	//------------------------GPIOA NSS------------------------------------------

	GPIOA_InitStructure.GPIO_Pin = SPI5_NSS_Pin;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOA_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOA_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIOA_InitStructure);

	GPIO_SetBits(SPI5_NSS_GPIO, SPI5_NSS_Pin);

	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;

	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
	SPI_Cmd(SPI1, ENABLE);

	positionOfPresentImages = getPositionOfPresentImagesInCarrousel();
	while (isBusy())
		;
	if (debug)
		Com1::getInstance()->sendString("\nFlash initialization completed.\n");

}

void Flash::setCS(bool state) {
	if (state) {
		GPIO_ResetBits(SPI5_NSS_GPIO, SPI5_NSS_Pin);
	} else {
		GPIO_SetBits(SPI5_NSS_GPIO, SPI5_NSS_Pin);
	}
}

uint8_t Flash::spiTransfer(uint8_t data) {
	uint8_t result = 0;
	while (SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET)
		;
	SPI1->DR = data;
	while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET)
		;
	result = SPI1->DR;
	return result;
}

void Flash::getDeviceID(uint8_t *buffer) {
	setCS(true);
	spiTransfer(DeviceID);
	for (int i = 0; i < 5; i++) {
		buffer[i] = spiTransfer(DummyByte);
		if (debug) {
			Com1::getInstance()->sendByte8ToBinaryString(buffer[i]);
			Com1::getInstance()->sendString("\n");
		}
	}
	setCS(false);
}

uint16_t Flash::readStatusRegister() {
	uint16_t a, b;
	setCS(true);
	spiTransfer(StatusRegisterRead);
	a = spiTransfer(DummyByte);
	b = spiTransfer(DummyByte);
	setCS(false);
	return a << 8 | b;
}

void Flash::readStatusRegisterToString() {
	uint16_t a, b;
	setCS(true);
	spiTransfer(StatusRegisterRead);
	a = spiTransfer(DummyByte);
	b = spiTransfer(DummyByte);
	setCS(false);
	if (debug) {
		(a & 0b10000000) ?
				Com1::getInstance()->sendString("b7 : RDY/BUSY Ready\n") :
				Com1::getInstance()->sendString("b7 : RDY/BUSY Busy\n");
		(a & 0b01000000) ?
				Com1::getInstance()->sendString("b6 : COMP Flase\n") :
				Com1::getInstance()->sendString("b6 : COMP true\n");
		(a & 0b00110100) ?
				Com1::getInstance()->sendString("b5:2 : DENSITY 32-Mbit\n") :
				Com1::getInstance()->sendString("b5:2 : DENSITY ???\n");
		(a & 0b00000010) ?
				Com1::getInstance()->sendString("b1 : PROTECT enable\n") :
				Com1::getInstance()->sendString("b1 : PROTECT disable\n");
		(a & 0b00000001) ?
				Com1::getInstance()->sendString("b0 : PAGE SIZE 512B\n") :
				Com1::getInstance()->sendString("b0 : PAGE SIZE 528B\n");
		(b & 0b10000000) ?
				Com1::getInstance()->sendString("b7 : RDY/BUSY Ready\n") :
				Com1::getInstance()->sendString("b7 : RDY/BUSY Busy\n");
		(b & 0b01000000) ?
				Com1::getInstance()->sendString("b6 : RES\n") :
				Com1::getInstance()->sendString("b6 : RES\n");
		(b & 0b00100000) ?
				Com1::getInstance()->sendString("b5 : EPE Detected\n") :
				Com1::getInstance()->sendString("b5 : EPE Ok\n");
		(b & 0b00100000) ?
				Com1::getInstance()->sendString("b4 : RES\n") :
				Com1::getInstance()->sendString("b4 : RES\n");
		(b & 0b00100000) ?
				Com1::getInstance()->sendString("b3 : SLE Enable\n") :
				Com1::getInstance()->sendString("b3 : SLE Disable\n");
		(b & 0b00100000) ?
				Com1::getInstance()->sendString("b2 : PS2 Is\n") :
				Com1::getInstance()->sendString("b2 : PS2 Not\n");
		(b & 0b00000010) ?
				Com1::getInstance()->sendString("b1 : PS1 Is\n") :
				Com1::getInstance()->sendString("b1 : PS2 Not\n");
		(b & 0b00000001) ?
				Com1::getInstance()->sendString("b0 : ES Is\n") :
				Com1::getInstance()->sendString("b0 : ES Not\n");
	}
}

uint8_t Flash::readConfigurationRegister() {
	uint8_t t = 0;
	setCS(true);
	spiTransfer(ConfigurationRegisterRead);
	t = spiTransfer(DummyByte);
	setCS(false);
	return t;
}

void Flash::setPageSizeBinary() {
	if (debug)
		Com1::getInstance()->sendString(
				"Configuring page size of 512 bytes (power of 2 adresses)\n");
	setCS(true);
	for (int i = 0; i < 4; i++) {
		spiTransfer(BinaryPageSize[i]);
	}
	setCS(false);
	while (isBusy())
		;
	if (debug)
		Com1::getInstance()->sendString("Configuration completed\n");
}

bool Flash::isBusy() {
	return !(readStatusRegister() & BusyFlag);
}

uint8_t Flash::readByte(const address_t *add) {
	uint8_t incommingByte;
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(MainMemmoryPageRead);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	spiTransfer(DummyByte);
	spiTransfer(DummyByte);
	spiTransfer(DummyByte);
	spiTransfer(DummyByte);
	incommingByte = spiTransfer(DummyByte);
	setCS(false);
	return incommingByte;
}

void Flash::readByte(const address_t *add, uint8_t *buffer, uint16_t nByte) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(MainMemmoryPageRead);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	spiTransfer(DummyByte);
	spiTransfer(DummyByte);
	spiTransfer(DummyByte);
	spiTransfer(DummyByte);

	for (uint16_t i = 0; i < nByte; i++) {
		buffer[i] = spiTransfer(DummyByte);
	}
	setCS(false);
}

void Flash::readPageArray(const address_t *add, uint8_t* buffer, uint32_t nBytes) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(ContinuousPageRead);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));

	for (uint32_t i = 0; i < nBytes; i++) {
		buffer[i] = spiTransfer(DummyByte);
	}
	setCS(false);
}

void Flash::writeByte(const address_t *add, uint8_t byte) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(WrtitePagesThroughBuf1BIE);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	spiTransfer(byte);
	setCS(false);

	while (!(readStatusRegister() & BusyFlag))
		;
}
void Flash::writeByte(const address_t *add, int byte) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(WrtitePagesThroughBuf1BIE);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	spiTransfer(byte);
	setCS(false);

	while (!(readStatusRegister() & BusyFlag))
		;
}
void Flash::writeByte(const address_t *add, uint16_t byte) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(WrtitePagesThroughBuf1BIE);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	for (int i = 0; i < 2; i++) {
		spiTransfer(byte >> (16 - (8 * i)));
	}
	setCS(false);

	while (!(readStatusRegister() & BusyFlag))
		;
}
void Flash::writeByte(const address_t* add, uint32_t byte) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(WrtitePagesThroughBuf1BIE);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	for (int i = 0; i < 4; i++) {
		spiTransfer(byte >> (24 - (8 * i)));
	}
	setCS(false);

	while (!(readStatusRegister() & BusyFlag))
		;
}

void Flash::writeByte(const address_t *add, uint8_t *byte, uint16_t nByte) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(WrtitePagesThroughBuf1BIE);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	for (uint16_t i = 0; i < nByte; i++) {
		spiTransfer(byte[i]);
	}
	setCS(false);

	while (!(readStatusRegister() & BusyFlag))
		;
}

void Flash::writeByte(const address_t *add, const char *byte, uint16_t nByte) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;
	address |= add->byte;

	setCS(true);
	spiTransfer(WrtitePagesThroughBuf1BIE);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	for (uint16_t i = 0; i < nByte; i++) {
		spiTransfer(byte[i]);
	}
	setCS(false);
	while (!(readStatusRegister() & BusyFlag))
		;
}

void Flash::erasePage(const address_t *add) {
	uint32_t address = 0;
	address = add->page;
	address = address << 9;

	if (debug)
		Com1::getInstance()->sendString("Erasing page...\n");

	setCS(true);
	spiTransfer(MainMemmoryPageRead);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));
	setCS(false);
	while (isBusy())
		;

	if (debug)
		Com1::getInstance()->sendString("Erase successful...\n");
}

void Flash::eraseChip() {
	if (debug)
		Com1::getInstance()->sendString("Erasing Chip...\n");

	setCS(true);
	spiTransfer(ChipErase[0]);
	spiTransfer(ChipErase[1]);
	spiTransfer(ChipErase[2]);
	spiTransfer(ChipErase[3]);
	setCS(false);
	while (!(readStatusRegister() & BusyFlag))
		;
	if (debug)
		Com1::getInstance()->sendString("Erase successful...\n");

}

uint32_t Flash::getPositionOfPresentImagesInCarrousel() {
	uint8_t bytes[4];
	uint32_t result;
	readPageArray(&PositionOfPresentImagesInCarrouselAddress, bytes, 4);
	result = bytes[0];
	result = (result << 8) | bytes[1];
	result = (result << 8) | bytes[2];
	result = (result << 8) | bytes[3];
	return result;
}

void Flash::savePositionOfPresentImagesInCarrousel() {
	writeByte(&PositionOfPresentImagesInCarrouselAddress, positionOfPresentImages);
}

uint8_t Flash::getNumberOfImagesInCarrousel() {
	return countSetBits(positionOfPresentImages);
}

bool Flash::getPixelColumn(uint8_t imageNo, uint8_t columnNo, uint8_t* spiBuffer1, uint8_t* spiBuffer2, uint8_t* spiBuffer3, uint8_t* spiBuffer4) {
	if (debug)
		Com1::getInstance()->sendString("Loading pixel column from flash...\n");

	imageNo = imageNo % MaxImageStored;

	uint32_t pixelColumnPageOffset = (columnNo * ColumnPixelArraySize) / PageSize;
	uint32_t pixelColumnStartPage = FirstImagePageAddress + ((imageNo - 1) * ImagePages);
	uint32_t pixelColumnStartByte = (columnNo * ColumnPixelArraySize) % PageSize;

	if (debug) {
		Com1::getInstance()->sendString("\npixelColumnPageOffset :");
		Com1::getInstance()->sendByte32ToBinaryString(pixelColumnPageOffset);
		Com1::getInstance()->sendString("pixelColumnStartPage :");
		Com1::getInstance()->sendByte32ToBinaryString(pixelColumnStartPage);
		Com1::getInstance()->sendString("pixelColumnStartByte :");
		Com1::getInstance()->sendByte32ToBinaryString(pixelColumnStartByte);
	}

	address_t add;
	add.page = pixelColumnStartPage + pixelColumnPageOffset;
	add.byte = pixelColumnStartByte;

	uint32_t address = 0;
	address = add.page;
	address = address << 9;
	address |= add.byte;

	setCS(true);
	spiTransfer(ContinuousPageRead);
	spiTransfer((address & 0x00FF0000) >> 16);
	spiTransfer((address & 0x0000FF00) >> 8);
	spiTransfer((address & 0x000000FF));

	for (uint32_t i = 0; i < SPIBufferSize; i++)
		spiBuffer1[i] = spiTransfer(DummyByte);
	for (uint32_t i = 0; i < SPIBufferSize; i++)
		spiBuffer2[i] = spiTransfer(DummyByte);
	for (uint32_t i = 0; i < SPIBufferSize; i++)
		spiBuffer3[i] = spiTransfer(DummyByte);
	for (uint32_t i = 0; i < SPIBufferSize; i++)
		spiBuffer4[i] = spiTransfer(DummyByte);

	setCS(false);

	if (debug)
		Com1::getInstance()->sendString("Column loaded from flash...\n");
	return true;
}

bool Flash::saveImageToFlash(uint8_t imageNo, const char* filename, uint8_t* source) {
	if (debug)
		Com1::getInstance()->sendString("Writing image to flash...\n");

	// Save BMP RAW Data to flash
	imageNo = imageNo % MaxImageStored;
	address_t add;
	add.byte = 0;
	for (int i = 0; i < ImagePages; i++) {
		add.page = (FirstImagePageAddress + ((imageNo - 1) * ImagePages)) + i;
		writeByte(&add, source, PageSize);
		while (isBusy())
			;
	}

	// Save image position flag to flash
	positionOfPresentImages |= 1 << (imageNo - 1);
	writeByte(&PositionOfPresentImagesInCarrouselAddress, positionOfPresentImages);

	//Save filename to flash
	add = FilenamePage;
	add.byte = (imageNo - 1) * FilenameSize;
	writeByte(&add, filename, FilenameSize);

	if (debug)
		Com1::getInstance()->sendString("Image written to flash...\n");

	return true;
}

bool Flash::loadImageFromFlash(uint8_t imageNo, uint8_t* destination) {
	if (debug)
		Com1::getInstance()->sendString("Loading image from flash...\n");
	imageNo = imageNo % MaxImageStored;
	address_t add;
	add.byte = 0;
	add.page = FirstImagePageAddress + ((imageNo - 1) * ImagePages);
	readPageArray(&add, destination, ImageFileSize);
	if (debug)
		Com1::getInstance()->sendString("Image loaded from flash...\n");
	return true;
}

void Flash::setDebug(bool debug) {
	this->debug = debug;
}

uint8_t Flash::countSetBits(uint32_t n) {
	uint8_t count = 0;
	while (n) {
		count += n & 1;
		n >>= 1;
	}
	return count;
}

void Flash::eraseImage(uint8_t imageNo) {
	positionOfPresentImages &= ~(1 << (imageNo - 1));
	writeByte(&PositionOfPresentImagesInCarrouselAddress, positionOfPresentImages);
}

void Flash::getFilename(uint8_t imageNo, uint8_t *destination) {
	imageNo = imageNo % 32;
	address_t add = FilenamePage;
	add.byte = (imageNo - 1) * FilenameSize;
	readByte(&add, destination, (uint32_t) FilenameSize);

}

void Flash::resetImageCount() {
	if (debug)
		Com1::getInstance()->sendString("Resetting number of images...\n");
	positionOfPresentImages = 0;
	savePositionOfPresentImagesInCarrousel();
	if (debug)
		Com1::getInstance()->sendString("Reset completed...\n");
}

