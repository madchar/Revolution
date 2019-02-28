/*
 * stm32spi1.cpp
 *
 *  Created on: Feb 28, 2019
 *      Author: rev
 */

#include "stm32spi1.hpp"

STM32SPI1::STM32SPI1()
{

}

STM32SPI1::~STM32SPI1()
{

}

void STM32SPI1::init()
{
	//------------------------GPIOA init------------------------------------------
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = SPI1_MOSI_Pin | SPI1_CLK_Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_Init(SPI1_MOSI_GPIO, &GPIO_InitStructure);

		GPIO_PinAFConfig(SPI1_MOSI_GPIO,SPI1_MOSI_PinSource,SPI1_ALTERNATE_FUNCTION);
		GPIO_PinAFConfig(SPI1_CLK_GPIO,SPI1_CLK_PinSource,SPI1_ALTERNATE_FUNCTION);

		//----------------------SPI init------------------------------------------------
		SPI_InitTypeDef SPI_InitStruct;
		SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
		SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStruct.SPI_CRCPolynomial = 10;
		SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
		SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB ;
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;

		SPI_Init(SPI1,&SPI_InitStruct);
}

void STM32SPI1::sendByte(uint16_t data)
{
	SPI_I2S_SendData(SPI1,data);
}

uint16_t STM32SPI1::receiveData()
{
	uint16_t temp;
	temp = SPI_I2S_ReceiveData(SPI1);
	return temp;
}
