/*
 * tlc5955.cpp
 *
 *  Created on: Mar 1, 2019
 *      Author: Zandwich
 */

#include "tlc5955.hpp"
#include <math.h>
#include <stdio.h>

TLC5955::TLC5955()
{

}

TLC5955::~TLC5955() {
}


void TLC5955::init()
{
	//---------------LATCH 1------------------------
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = TLC_LAT1_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(TLC_LAT1_GPIO, &GPIO_InitStructure);
	//---------------LATCH 2------------------------
	GPIO_InitStructure.GPIO_Pin = TLC_LAT2_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(TLC_LAT2_GPIO, &GPIO_InitStructure);

	// Set default color channel indicies
	setRGBPinOrder(rgb_order_default[0], rgb_order_default[1], rgb_order_default[2]);

	//spi1.init();
	//spi1.setBitBang();

}

void TLC5955::updateControl()
{
	spi1.setBitBang();

	for (int8_t repeatCtr = 0; repeatCtr < CONTROL_WRITE_COUNT; repeatCtr++)
	{
		for (int8_t chip = tlc_count - 1; chip >= 0; chip--)
		{
			/*if (debug >= 2)
	        Serial.println(F("Starting Control Mode... %s"));*/

			_buffer_count = 7;
			//resetAllLatch();
			setControlModeBit(CONTROL_MODE_ON);

			// Add CONTROL_ZERO_BITS blank bits to get to correct position for DC/FC
			for (int16_t a = 0; a < (CONTROL_ZERO_BITS); a++)
				setBuffer(0);
			// 5-bit Function Data
			for (int8_t a = FC_BITS - 1; a >= 0; a--)
				setBuffer((_function_data & (1 << a)));
			// Blue Brightness
			for (int8_t a = GB_BITS - 1; a >= 0; a--)
				setBuffer((_bright_blue & (1 << a)));
			// Green Brightness
			for (int8_t a = GB_BITS - 1; a >= 0; a--)
				setBuffer((_bright_green & (1 << a)));
			// Red Brightness
			for (int8_t a = GB_BITS - 1; a >= 0; a--)
				setBuffer((_bright_red & (1 << a)));
			// Maximum Current Data
			for (int8_t a = MC_BITS - 1; a >= 0; a--)
				setBuffer((_Max_Current_Blue & (1 << a)));
			for (int8_t a = MC_BITS - 1; a >= 0; a--)
				setBuffer((_Max_Current_Green & (1 << a)));
			for (int8_t a = MC_BITS - 1; a >= 0; a--)
				setBuffer((_Max_Current_Red & (1 << a)));

			// Dot Correction data
			for (int8_t a = LEDS_PER_CHIP - 1; a >= 0; a--)
			{
				for (int8_t b = COLOR_CHANNEL_COUNT - 1; b >= 0; b--)
				{
					for (int8_t c = 6; c >= 0; c--)
						setBuffer(dot_correction_data[chip][a][b] & (1 << c));
				}
			}

		}

		latch(true);
		latch(false);

	}
	spi1.init();

}

void TLC5955::updateLeds()
{

	/*if (force_max_current)
	{
		// Get number of counts for current pattern
		uint32_t power_output_counts = 0;
		for (int16_t chip = (int8_t)(tlc_count) - 1; chip >= 0; chip--)
			for (int8_t led_channel_index = (int8_t)LEDS_PER_CHIP - 1; led_channel_index >= 0; led_channel_index--)
				for (int8_t color_channel_index = (int8_t)COLOR_CHANNEL_COUNT - 1; color_channel_index >= 0; color_channel_index--)
					power_output_counts += grayscale_data[chip][led_channel_index][color_channel_index];

		double power_output_amps = ((double)power_output_counts / (double)UINT16_MAX) * LED_CURRENT_AMPS;

	}*/
	/* if (debug >= 2)
	  {
	    Serial.println(F("Begin LED Update String (All Chips)..."));
	  }*/

	// uint32_t power_output_counts = 0;
	for (int16_t chip = (int8_t)tlc_count - 1; chip >= 0; chip--)
	{
		setControlModeBit(CONTROL_MODE_OFF);

		uint8_t color_channel_ordered;
		for (int8_t led_channel_index = (int8_t)LEDS_PER_CHIP - 1; led_channel_index >= 0; led_channel_index--)
		{
			for (int8_t color_channel_index = (int8_t)COLOR_CHANNEL_COUNT - 1; color_channel_index >= 0; color_channel_index--)
			{
				color_channel_ordered = rgb_order[chip][led_channel_index][(uint8_t) color_channel_index];

				spi1.sendByte16((char)(grayscale_data[chip][led_channel_index][color_channel_ordered]));
				//spi1.sendByte8((char)(grayscale_data[chip][led_channel_index][color_channel_ordered] >> 8));		// Output MSB first
				//spi1.sendByte8((char)(grayscale_data[chip][led_channel_index][color_channel_ordered] & 0xFF));	// Followed by LSB

				//spi2.sendByte((char)(grayscale_data[chip+3][led_channel_index][color_channel_ordered] >> 8));		// Output MSB first
				//spi2.sendByte((char)(grayscale_data[chip+3][led_channel_index][color_channel_ordered] & 0xFF));	// Followed by LSB

				//spi3.sendByte((char)(grayscale_data[chip+6][led_channel_index][color_channel_ordered] >> 8));		// Output MSB first
				//spi3.sendByte((char)(grayscale_data[chip+6][led_channel_index][color_channel_ordered] & 0xFF));	// Followed by LSB

				//spi4.sendByte((char)(grayscale_data[chip+9][led_channel_index][color_channel_ordered] >> 8));		// Output MSB first
				//spi4.sendByte((char)(grayscale_data[chip+9][led_channel_index][color_channel_ordered] & 0xFF));	// Followed by LSB
			}
		}

	}
	/*
	  if (debug >= 2)
	  {
	    printf("End LED Update String (All Chips)");
	  }*/
	//latch(true);


}

void TLC5955::latch(bool lat)
{
	if (lat)
	{
		GPIO_ResetBits(TLC_LAT1_GPIO,TLC_LAT1_Pin);
		for(int i = 0; i<10;i++)
		{
			asm("nop");
		}
		GPIO_SetBits(TLC_LAT1_GPIO,TLC_LAT1_Pin);
		for(int i = 0; i<10;i++)
		{
			asm("nop");
		}
		GPIO_ResetBits(TLC_LAT1_GPIO,TLC_LAT1_Pin);
	}
	else
	{
		GPIO_ResetBits(TLC_LAT2_GPIO,TLC_LAT2_Pin);
		for(int i = 0; i<10;i++)
		{
			asm("nop");
		}
		GPIO_SetBits(TLC_LAT2_GPIO,TLC_LAT2_Pin);
		for(int i = 0; i<10;i++)
		{
			asm("nop");
		}
		GPIO_ResetBits(TLC_LAT2_GPIO,TLC_LAT2_Pin);
	}
}

void TLC5955::resetAllLatch()
{
	GPIO_ResetBits(TLC_LAT1_GPIO,TLC_LAT1_Pin);
	GPIO_ResetBits(TLC_LAT2_GPIO,TLC_LAT2_Pin);
}

void TLC5955::setBuffer(uint8_t bit)
{
	bitWrite(_buffer, _buffer_count, bit);
	_buffer_count--;
	if (_buffer_count == -1)
	{
		/*if (debug >= 2)
	      printf(_buffer);*/

		spi1.sendManualByte(_buffer);
		//spi2.sendByte(_buffer);
		//spi3.sendByte(_buffer);
		//spi4.sendByte(_buffer);
		_buffer_count = 7;
		_buffer = 0;
	}
}

void TLC5955::setControlModeBit(bool isControlMode)
{
	if (isControlMode)
	{
		//spi1.setBitBang();
		//spi2.setBitBang();
		//spi3.setBitBang();
		//spi4.setBitBang();

		//latch(true);
		//latch(false);

		spi1.sendControlBits();
		//spi2.sendControlBits();
		//spi3.sendControlBits();
		//spi4.sendControlBits();

		//spi1.init();
		//spi2.init();
		//spi3.init();
		//spi4.init();

	}
	else
	{

		spi1.setBitBang();

		GPIO_ResetBits(SPI1_MOSI_GPIO,SPI1_MOSI_Pin);

		GPIO_ResetBits(SPI1_CLK_GPIO,SPI1_CLK_Pin);

		GPIO_SetBits(SPI1_CLK_GPIO,SPI1_CLK_Pin);

		GPIO_ResetBits(SPI1_CLK_GPIO,SPI1_CLK_Pin);

		spi1.init();

		//spi1.sendByte8(0x00);
		//spi2.sendByte(0x00);
		//spi3.sendByte(0x00);
		//spi4.sendByte(0x00);
	}
}

void TLC5955::flushBuffer()
{

}

void TLC5955::deassertAll()
{

}

void TLC5955::assertAll()
{

}

void TLC5955::setBitBangConfig()
{
	spi1.setBitBang();
	//spi2.setBitBang();
	//spi3.setBitBang();
	//spi4.setBitBang();
}

void TLC5955::setLedRGB(uint16_t led_number, uint16_t red, uint16_t green,uint16_t blue)
{
	uint8_t chip = (uint16_t)floor(led_number / LEDS_PER_CHIP);
	uint8_t channel = (uint8_t)(led_number - LEDS_PER_CHIP * chip);
	grayscale_data[chip][channel][2] = blue;
	grayscale_data[chip][channel][1] = green;
	grayscale_data[chip][channel][0] = red;
}

void TLC5955::setLedRGB(uint16_t led_number, uint16_t rgb)
{
	uint8_t chip = (uint16_t)floor(led_number / LEDS_PER_CHIP);
	uint8_t channel = (uint8_t)(led_number - LEDS_PER_CHIP * chip);
	grayscale_data[chip][channel][2] = rgb;
	grayscale_data[chip][channel][1] = rgb;
	grayscale_data[chip][channel][0] = rgb;
}

void TLC5955::setAllLedsRGB(uint16_t red, uint16_t green, uint16_t blue)
{
	if (COLOR_CHANNEL_COUNT == 3)
	{
		for (int8_t chip = tlc_count - 1; chip >= 0; chip--)
		{
			for (int8_t channel = 0; channel < LEDS_PER_CHIP; channel++)
			{
				grayscale_data[chip][channel][2] = blue;
				grayscale_data[chip][channel][1] = green;
				grayscale_data[chip][channel][0] = red;
			}
		}
	}// else
		//printf("ERROR (TLC5955::setAllLedRgb): Color channel count is not 3");
}

void TLC5955::setAllLed(uint16_t gsvalue)
{
	for (int8_t chip = tlc_count - 1; chip >= 0; chip--)
	{
		for (int8_t a = 0; a < LEDS_PER_CHIP; a++)
		{
			for (int8_t b = 0; b < COLOR_CHANNEL_COUNT; b++)
				grayscale_data[chip][a][b] = gsvalue;
		}
	}
}

void TLC5955::setLedAppend(uint16_t led_number, uint16_t red, uint16_t green,uint16_t blue)
{
	uint8_t chip = (uint16_t)floor(led_number / LEDS_PER_CHIP);
	uint8_t channel = (uint8_t)(led_number - LEDS_PER_CHIP * chip);        // Turn that LED on

	if (((uint32_t)blue + (uint32_t) grayscale_data[chip][channel][2]) > (uint32_t)UINT16_MAX)
		grayscale_data[chip][channel][2] = UINT16_MAX;
	else
		grayscale_data[chip][channel][2] = blue  + grayscale_data[chip][channel][2];

	if (((uint32_t)green + (uint32_t) grayscale_data[chip][channel][1]) > (uint32_t)UINT16_MAX)
		grayscale_data[chip][channel][1] = UINT16_MAX;
	else
		grayscale_data[chip][channel][1] = green  + grayscale_data[chip][channel][1];

	if (((uint32_t)red + (uint32_t)grayscale_data[chip][channel][0]) > (uint32_t)UINT16_MAX)
		grayscale_data[chip][channel][0] = UINT16_MAX;
	else
		grayscale_data[chip][channel][0] = red  + grayscale_data[chip][channel][0];
}

void TLC5955::setChannel(uint16_t channel_number, uint16_t value)
{
	// Change to multi-channel indexing
	int16_t chip_number = floor(channel_number / (COLOR_CHANNEL_COUNT * LEDS_PER_CHIP));
	int16_t channel_number_new = (int16_t) floor((channel_number - chip_number * LEDS_PER_CHIP * COLOR_CHANNEL_COUNT) / COLOR_CHANNEL_COUNT);
	int16_t color_channel_number = (int16_t) (channel_number - chip_number * LEDS_PER_CHIP * COLOR_CHANNEL_COUNT) % COLOR_CHANNEL_COUNT;

	// Set grayscale data
	grayscale_data[chip_number][channel_number_new][color_channel_number] = value;
}

void TLC5955::setLedDotCorrection(uint16_t led_number,uint8_t color_channel_index, uint8_t dc_value)
{
	if (color_channel_index < COLOR_CHANNEL_COUNT)
	{
		uint8_t chip = (uint16_t)floor(led_number / LEDS_PER_CHIP);
		uint8_t channel = (uint8_t)(led_number - LEDS_PER_CHIP * chip);
		dot_correction_data[chip][channel][color_channel_index] = dc_value;
	}
	else printf("ERROR (TLC5955::setLedDotCorrection) : Invalid color channel index");
}

void TLC5955::setMaxCurrent(uint8_t red, uint8_t green, uint8_t blue)
{
	if (red > 7) red = 7;
	_Max_Current_Red = red;

	if (green > 7) green = 7;
	_Max_Current_Green = green;

	if (blue > 7) blue = 7;
	_Max_Current_Blue = blue;
}

void TLC5955::setMaxCurrent(uint8_t MCRGB)
{
	// Ensure max Current agrees with datasheet (3-bit)
	if (MCRGB > 7) MCRGB = 7;
	_Max_Current_Red = MCRGB;
	_Max_Current_Green = MCRGB;
	_Max_Current_Blue = MCRGB;
}

void TLC5955::setBrightnessCurrent(uint8_t rgb)
{
	_bright_red = rgb;
	_bright_green = rgb;
	_bright_blue = rgb;
}

void TLC5955::setBrightnessCurrent(uint8_t red, uint8_t green, uint8_t blue)
{
	_bright_red = red;
	_bright_green = green;
	_bright_blue = blue;
}

void TLC5955::setFunctionControlData(bool DSPRPT, bool TMGRST, bool RFRESH,bool ESPWM, bool LSDVLT)
{
	uint8_t data = 0;
	data |= DSPRPT << 0;
	data |= TMGRST << 1;
	data |= RFRESH << 2;
	data |= ESPWM << 3;
	data |= LSDVLT << 4;
	_function_data = data;
}

void TLC5955::setRGBPinOrder(uint8_t red_pos, uint8_t green_pos,uint8_t blue_pos)
{
	if (COLOR_CHANNEL_COUNT == 3)
	{
		for (int8_t chip = tlc_count - 1; chip >= 0; chip--)
		{
			for (int8_t channel = 0; channel < LEDS_PER_CHIP; channel++)
			{
				rgb_order[chip][channel][0] = red_pos;
				rgb_order[chip][channel][1] = green_pos;
				rgb_order[chip][channel][2] = blue_pos;
			}
		}
	} //else
		//printf("ERROR (TLC5955::setRgbPinOrder): Color channel count is not 3");
}

void TLC5955::setSinglePinOrder(uint16_t led_number, uint8_t color_channel_index,uint8_t position)
{
	uint8_t chip = (uint16_t)floor(led_number / LEDS_PER_CHIP);
	uint8_t channel = (uint8_t)(led_number - LEDS_PER_CHIP * chip);
	rgb_order[chip][channel][color_channel_index] = position;
}

void TLC5955::setSingleRGBPinOrder(uint16_t led_number, uint8_t red_pos,uint8_t green_pos, uint8_t blue_pos)
{
	uint8_t chip = (uint16_t)floor(led_number / LEDS_PER_CHIP);
	uint8_t channel = (uint8_t)round(led_number - LEDS_PER_CHIP * chip);
	rgb_order[chip][channel][0] = red_pos;
	rgb_order[chip][channel][1] = green_pos;
	rgb_order[chip][channel][2] = blue_pos;
}

void TLC5955::setAllDcData(uint8_t dcvalue)
{
  for (int8_t chip = tlc_count - 1; chip >= 0; chip--)
  {
    for (int8_t a = LEDS_PER_CHIP - 1; a >= 0; a--)
    {
      for (int8_t b = COLOR_CHANNEL_COUNT - 1; b >= 0; b--)
        dot_correction_data[chip][a][b] = dcvalue;
    }
  }
}
