/*
 * hardware.h
 *
 *  Created on: Feb 28, 2019
 *      Author: Zandwich
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

//*************************SPI****************************************
//-------------------------SPI1----------------------
#define SPI1_ALTERNATE_FUNCTION		GPIO_AF_SPI1

#define SPI1_MOSI_GPIO				GPIOA
#define SPI1_MOSI_PinSource			GPIO_PinSource7
#define SPI1_MOSI_Pin				GPIO_Pin_7

#define SPI1_CLK_GPIO				GPIOA
#define SPI1_CLK_PinSource			GPIO_PinSource5
#define SPI1_CLK_Pin				GPIO_Pin_5
//-------------------------SPI2----------------------
#define SPI2_ALTERNATE_FUNCTION		GPIO_AF_SPI2

#define SPI2_MOSI_GPIO				GPIOC
#define SPI2_MOSI_PinSource			GPIO_PinSource3
#define SPI2_MOSI_Pin				GPIO_Pin_3

#define SPI2_CLK_GPIO				GPIOB
#define SPI2_CLK_PinSource			GPIO_PinSource10
#define SPI2_CLK_Pin				GPIO_Pin_10
//-------------------------SPI3----------------------
#define SPI3_ALTERNATE_FUNCTION		GPIO_AF_SPI3

#define SPI3_MOSI_GPIO				GPIOC
#define SPI3_MOSI_PinSource			GPIO_PinSource12
#define SPI3_MOSI_Pin				GPIO_Pin_12

#define SPI3_CLK_GPIO				GPIOB
#define SPI3_CLK_PinSource			GPIO_PinSource12
#define SPI3_CLK_Pin				GPIO_Pin_12
//-------------------------SPI4----------------------
#define SPI4_ALTERNATE_FUNCTION		GPIO_AF_SPI4

#define SPI4_MOSI_GPIO				GPIOA
#define SPI4_MOSI_PinSource			GPIO_PinSource1
#define SPI4_MOSI_Pin				GPIO_Pin_1

#define SPI4_CLK_GPIO				GPIOB
#define SPI4_CLK_PinSource			GPIO_PinSource13
#define SPI4_CLK_Pin				GPIO_Pin_13
//-------------------------SPI5----------------------
#define SPI5_ALTERNATE_FUNCTION		GPIO_AF6_SPI5

#define SPI5_MOSI_GPIO				GPIOA
#define SPI5_MOSI_PinSource			GPIO_PinSource10
#define SPI5_MOSI_Pin				GPIO_Pin_10

#define SPI5_MISO_GPIO				GPIOA
#define SPI5_MISO_PinSource			GPIO_PinSource12
#define SPI5_MISO_Pin				GPIO_Pin_12

#define SPI5_CLK_GPIO				GPIOB
#define SPI5_CLK_PinSource			GPIO_PinSource0
#define SPI5_CLK_Pin				GPIO_Pin_0

#define SPI5_NSS_GPIO				GPIOB
#define SPI5_NSS_PinSource			GPIO_PinSource1
#define SPI5_NSS_Pin				GPIO_Pin_1


#endif /* HARDWARE_H_ */
