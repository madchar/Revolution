/*
 * stm32spi2.cpp
 *
 *  Created on: Mar 1, 2019
 *      Author: Zandwich
 */

#include "stm32spi2.hpp"

STM32SPI2::STM32SPI2()
{

}

STM32SPI2::~STM32SPI2()
{

}

void STM32SPI2::init()
{
	//------------------------GPIOB------------------------------------------
	GPIO_InitTypeDef GPIOB_InitStructure;
	GPIOB_InitStructure.GPIO_Pin = SPI2_CLK_Pin;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOB_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOB_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(SPI2_CLK_GPIO, &GPIOB_InitStructure);

	GPIO_PinAFConfig(SPI2_CLK_GPIO,SPI2_CLK_PinSource,SPI2_ALTERNATE_FUNCTION);

	//------------------------GPIOC------------------------------------------
	GPIO_InitTypeDef GPIOC_InitStructure;
	GPIOC_InitStructure.GPIO_Pin = SPI2_MOSI_Pin;
	GPIOC_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOC_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOC_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOC_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(SPI2_MOSI_GPIO, &GPIOC_InitStructure);

	GPIO_PinAFConfig(SPI2_MOSI_GPIO,SPI2_MOSI_PinSource,SPI2_ALTERNATE_FUNCTION);

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

	SPI_Init(SPI2,&SPI_InitStruct);
}

void STM32SPI2::sendByte(uint16_t data)
{
	assert();
	SPI_NSSInternalSoftwareConfig(SPI2,SPI_NSSInternalSoft_Set);
	SPI_SendData(SPI2,data);
	while(SPI_GetFlagStatus(SPI2,SPI_FLAG_TXE)==RESET);
	SPI_NSSInternalSoftwareConfig(SPI2,SPI_NSSInternalSoft_Reset);
	deassert();
}

uint16_t STM32SPI2::receiveData()
{
	assert();
	uint16_t temp;
	temp = SPI_ReceiveData(SPI2);
	deassert();
	return temp;
}

void STM32SPI2::assert()
{
	SPI_Cmd(SPI2,ENABLE);
}
void STM32SPI2::deassert()
{
	SPI_Cmd(SPI2,DISABLE);
}


