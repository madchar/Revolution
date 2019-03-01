/*
 * stm32spi5.cpp
 *
 *  Created on: Mar 1, 2019
 *      Author: Zandwich
 */

#include "stm32spi5.hpp"

STM32SPI5::STM32SPI5()
{

}

STM32SPI5::~STM32SPI5()
{

}

void STM32SPI5::init()
{
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
	//------------------------GPIOB------------------------------------------
	GPIO_InitTypeDef GPIOB_InitStructure;
	GPIOB_InitStructure.GPIO_Pin = SPI5_CLK_Pin | SPI5_NSS_Pin;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOB_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOB_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOB, &GPIOB_InitStructure);

	GPIO_PinAFConfig(SPI5_CLK_GPIO,SPI5_CLK_PinSource,SPI5_ALTERNATE_FUNCTION);
	GPIO_PinAFConfig(SPI5_NSS_GPIO,SPI5_NSS_PinSource,SPI5_ALTERNATE_FUNCTION);

	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CRCPolynomial = 10;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB ;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;

	SPI_Init(SPI5,&SPI_InitStruct);
}

void STM32SPI5::sendByte(uint16_t data)
{
	assert();
	SPI_SSOutputCmd(SPI5,ENABLE);
	SPI_SendData(SPI5,data);
	while(SPI_GetFlagStatus(SPI5,SPI_FLAG_TXE)==RESET);
	SPI_SSOutputCmd(SPI5,DISABLE);
	deassert();
}

uint16_t STM32SPI5::receiveData()
{
	uint16_t temp;
	assert();
	SPI_SSOutputCmd(SPI5,ENABLE);
	temp = SPI_ReceiveData(SPI5);
	while(SPI_GetFlagStatus(SPI5,SPI_FLAG_RXNE)!=RESET); //*******************À réviser********************
	SPI_SSOutputCmd(SPI5,DISABLE);
	deassert();
	return temp;
}

void STM32SPI5::assert()
{
	SPI_Cmd(SPI5,ENABLE);
}
void STM32SPI5::deassert()
{
	SPI_Cmd(SPI5,DISABLE);
}





