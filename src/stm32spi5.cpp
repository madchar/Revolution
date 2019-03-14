/*
 * stm32spi5.cpp
 *
 *  Created on: Mar 1, 2019
 *      Author: Zandwich
 */

#include "stm32spi5.hpp"
#include <string.h>

STM32SPI5::STM32SPI5()
{

}

STM32SPI5::~STM32SPI5()
{

}

void STM32SPI5::init()
{
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


	//------------------------GPIOA------------------------------------------
	GPIO_InitTypeDef GPIOA_InitStructure;
	GPIOA_InitStructure.GPIO_Pin = SPI5_MOSI_Pin | SPI5_MISO_Pin;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOA_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOA_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIOA_InitStructure);

	GPIO_PinAFConfig(SPI5_MOSI_GPIO,SPI5_MOSI_PinSource,SPI5_ALTERNATE_FUNCTION);
	GPIO_PinAFConfig(SPI5_MISO_GPIO,SPI5_MISO_PinSource,SPI5_ALTERNATE_FUNCTION);


	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB ;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;

	SPI_Init(SPI5,&SPI_InitStruct);
	GPIO_Write(SPI5_NSS_GPIO,SPI5_NSS_Pin);

}

void STM32SPI5::setCS(bool state)
{
	if (state) GPIO_Write(SPI5_NSS_GPIO,SPI5_NSS_Pin);
	else GPIO_ResetBits(SPI5_NSS_GPIO,SPI5_NSS_Pin);
}

void STM32SPI5::sendByte(uint16_t data)
{
	assert();
	setCS(true);
	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);
	SPI_SendData(SPI5,data);
	while(SPI_GetFlagStatus(SPI5,SPI_FLAG_TXE)==RESET);
	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Reset);
	setCS(false);
	deassert();
}

uint16_t STM32SPI5::receiveData()
{
	uint16_t result = 0;
	while(SPI_GetFlagStatus(SPI5,SPI_FLAG_RXNE)==RESET)
	result = SPI5->DR;
	return result;
}

void STM32SPI5::assert()
{
	SPI_Cmd(SPI5,ENABLE);
}
void STM32SPI5::deassert()
{
	SPI_Cmd(SPI5,DISABLE);
}

void STM32SPI5::readPage(uint32_t address, uint32_t nPage, uint8_t *buffer)
{

}

void STM32SPI5::getDeviceID(uint8_t *buffer)
{
	assert();

	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);
	setCS(false);
	SPI5->DR = DeviceID;
	while(SPI_GetFlagStatus(SPI5,SPI_FLAG_TXE)==RESET);
	for (int i = 0;i<5; i++)
	{
		while(SPI_GetFlagStatus(SPI5,SPI_FLAG_RXNE)==RESET) SPI5->DR = 0x00;
		buffer[i] = SPI5->DR;
	}
	setCS(true);
	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Reset);

	deassert();
}

void STM32SPI5::writePage(uint16_t address, uint16_t nPage, uint16_t *data)
{
	/*assert();

	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);
	setCS(false);

	SPI5->DR = WritePage;
	while(SPI_GetFlagStatus(SPI5,SPI_FLAG_TXE)==RESET);
	SPI5->DR = address;
	for (int i = 0;i<5; i++)
	{
		while(SPI_GetFlagStatus(SPI5,SPI_FLAG_RXNE)==RESET) SPI5->DR = 0x00;
		buffer[i] = SPI5->DR;
	}
	setCS(true);
	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Reset);

	deassert();*/
}

void STM32SPI5::formatData()
{

}



