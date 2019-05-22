/**
   TLC5955 Control Library
   Used to control the TI TLC5955 LED driver chip
   Zack Phillips - zkphil@berkeley.edu
   Product Page: http://www.ti.com/product/tlc5955
   Copyright (c) 2015, Zachary F. Phillips
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
 * Neither the name of Zack Phillips / UC Berkeley nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL Z. PHILLIPS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * tlc5955.hpp
 *
 *  Created on: Mar 1, 2019
 *      Author: Modifications par Matthieu Madran
 *        Desc: Contrôle du chip TLC5955
 */
#ifndef TLC5955_HPP_
#define TLC5955_HPP_

#include "hardware.h"

#include "stm32spi1.hpp"
#include "stm32spi2.hpp"
#include "stm32spi3.hpp"
#include "stm32spi4.hpp"

#define GS_BITS 16
#define GB_BITS 7
#define MC_BITS 3
#define FC_BITS 5
#define DC_BITS 7
#define CONTROL_ZERO_BITS 389   /* Bits required for correct control reg size */
#define TOTAL_REGISTER_SIZE 769
#define LATCH_DELAY 10
#define CONTROL_WRITE_COUNT 2
#define CONTROL_MODE_ON 1
#define CONTROL_MODE_OFF 0

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

// LED Current OUTPUT
static const float LED_CURRENT_AMPS = 0.020;

// Line ending for serial output
static const char LINE_ENDING[] = "\n";

class TLC5955 {
public:
	TLC5955();
	~TLC5955();

	//-------------Initialization---------------------------------------------------------------------------------
	/**
	 * Initialisation du TLC5955
	 * @param spi_1	Pointeur pour le spi1
	 * @param spi_2	Pointeur pour le spi2
	 * @param spi_3	Pointeur pour le spi3
	 * @param spi_4	Pointeur pour le spi4
	 */
	void init(STM32SPI1 *spi_1, STM32SPI2 *spi_2, STM32SPI3 *spi_3, STM32SPI4 *spi_4);


	//-------------Send data to device(update, flush,latch)-------------------------------------------------------
	/**
	 *	Envoyer les données du registre de contrôle au chip
	 */
	void updateControl();
	/**
	 * Envoyer les données pour l'affichage des LEDs
	 * @param buffer1 Buffer1 pour le spi1
	 * @param buffer2 Buffer2 pour le spi2
	 * @param buffer3 Buffer3 pour le spi3
	 * @param buffer4 Buffer4 pour le spi4
	 */
	void updateLeds(uint8_t *buffer1,uint8_t *buffer2,uint8_t *buffer3,uint8_t *buffer4);
	/**
	 * Envoyer un signal de latch au chip
	 * @param lat Sélectionne le port à envoyer le latch
	 */
	void latch(bool lat);
	/**
	 * Envoyer un byte simultanément dans les quatres buffer
	 * @param bit
	 */
	void setBuffer(uint8_t bit);
	/**
	 * Envoyer une requête pour régler le chip en mode contrôle
	 * @param isControlMode	Vrai ou faux
	 */
	void setControlModeBit(bool isControlMode);

	//-------------Set LEDS intensities---------------------------------------------------------------------------
	/**
	 * Régler la couleur d'une led spécifique
	 * @param led_number	Numéro de la led à régler
	 * @param red			Niveau de rouge
	 * @param green			Niveau de vert
	 * @param blue			Niveau de bleu
	 */
	void setLedRGB(uint16_t led_number,uint16_t red, uint16_t green, uint16_t blue);
	/**
	 * Régler la couleur d'une led spécifique
	 * @param led_number	Numéro de la led à régler
	 * @param rgb			Niveau pour le rouge,vert et bleu
	 */
	void setLedRGB(uint16_t led_number,uint16_t rgb);
	/**
	 * Régler la couleur de chaque leds
	 * @param red		Niveau de rouge
	 * @param green		Niveau de vert
	 * @param blue		Niveau de bleu
	 */
	void setAllLedsRGB(uint16_t red, uint16_t green, uint16_t blue);

	//-------------Control Mode Parameters------------------------------------------------------------------------
	/**
	 *	Régler le niveau de luminosité maximum pour chaque leds
	 * @param dcvalue		Niveau de luminosité sur 127
	 */
	void setAllDcData(uint8_t dcvalue);
	/**
	 * Régler le niveau de courant maximum pour chaque canaux de couleur
	 * @param red		Niveau de courant pour le rouge
	 * @param green		Niveau de courant pour le vert
	 * @param blue		Niveau de courant le bleu
	 */
	void setMaxCurrent(uint8_t red, uint8_t green, uint8_t blue);
	/**
	 * Régler le même niveau de courant maximum pour chaque canaux de couleur
	 * @param MCRGB		Niveau de courant maximum
	 */
	void setMaxCurrent(uint8_t MCRGB);
	/**
	 * Régler le niveau de luminosité pour chaque couleurs basé sur la luminosité maximum
	 * @param rgb		Niveau de luminosité RGB
	 */
	void setBrightnessCurrent(uint8_t rgb);
	/**
	 *  Régler le niveau de luminosité pour chaque canal de couleur individuel basé sur la luminosité maximum
	 * @param red		Niveau de luminosité pour le rouge
	 * @param green		Niveau de luminosité pour le vert
	 * @param blue		Niveau de luminosité pour le bleu
	 */
	void setBrightnessCurrent(uint8_t red, uint8_t green, uint8_t blue);
	/**
	 * Régler les modes de contrôle du chip, voir datasheet p.23
	 * @param DSPRPT	"Auto display repeat mode enable bit"
	 * @param TMGRST	"Display timing reset mode enable bit"
	 * @param RFRESH	"Auto data refresh mode enable bit"
	 * @param ESPWM		"ES-PWM mode enable bit"
	 * @param LSDVLT	"LSD detection voltage selection bit"
	 */
	void setFunctionControlData(bool DSPRPT, bool TMGRST, bool RFRESH, bool ESPWM, bool LSDVLT);
	/**
	 * Régler l'ordre des broches de couleur (0 à 2)
	 * @param red_pos		Position des rouges
	 * @param green_pos		Position des verts
	 * @param blue_pos		Position des bleus
	 */
	void setRGBPinOrder(uint8_t red_pos, uint8_t green_pos, uint8_t blue_pos);

	//-------------SPI FUNCTIONS----------------------------------------------------------------------------------
	/**
	 * Régler les GPIOs de SPI en mode de contrôle manuel
	 */
	void setBitBangConfig();

	static const uint8_t tlc_count = 12; // This
	static const uint8_t COLOR_CHANNEL_COUNT = 3;
	static const uint8_t LEDS_PER_CHIP = 16;
	static const uint8_t SPI_COUNT = 4;
	static bool force_max_current;
	static float max_current_amps;

	uint8_t dot_correction_data[tlc_count][LEDS_PER_CHIP][COLOR_CHANNEL_COUNT];
	uint8_t rgb_order[tlc_count][LEDS_PER_CHIP][COLOR_CHANNEL_COUNT];
	uint16_t grayscale_data[tlc_count][LEDS_PER_CHIP][COLOR_CHANNEL_COUNT];

	uint8_t rgb_order_default[3] = {0, 1, 2};

private:
	int debug = 0;

	uint8_t _function_data;
	uint16_t _bright_red;
	uint16_t _bright_green;
	uint16_t _bright_blue;
	uint8_t _Max_Current_Red = 127;
	uint8_t _Max_Current_Green = 127;
	uint8_t _Max_Current_Blue = 127;

	/* SPI */
	uint8_t _buffer;
	int8_t _buffer_count = 7;
	STM32SPI1 *spi1;
	STM32SPI2 *spi2;
	STM32SPI3 *spi3;
	STM32SPI4 *spi4;
};



#endif /* TLC5955_HPP_ */
