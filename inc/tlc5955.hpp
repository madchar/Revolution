/*
 * tlc5955.hpp
 *
 *  Created on: Mar 1, 2019
 *      Author: Zandwich
 */

#ifndef TLC5955_HPP_
#define TLC5955_HPP_

#include "ileddriver.hpp"
#include "hardware.h"
#include "spimux.hpp"

/* Bit Quantities (Change to match other TLC driver chips) */
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

// Serial baud rate
#define SPI_BAUD_RATE 2000000

// LED Current OUTPUT
static const float LED_CURRENT_AMPS = 0.020;

// Line ending for serial output
static const char LINE_ENDING[] = "\n";

class TLC5955: public ILedDriver {

	TLC5955::TLC5955(SPIMUX *mux);
	TLC5955::~TLC5955();

	//-------------Initialization---------------------------------------------------------------------------------
	void init();


	//-------------Send data to device(update, flush,latch)-------------------------------------------------------
	void updateControl(SPIMUX *spimux);
	void updateLeds(SPIMUX *spimux);
	void latch();
	void setBuffer(uint8_t bit);
	void setControlModeBit(bool isControlMode);
	void flushBuffer();

	//-------------Set LEDS intensities---------------------------------------------------------------------------
	void setLedRGB(uint16_t led_number,uint16_t red, uint16_t green, uint16_t blue);
	void setLedRGB(uint16_t led_number,uint16_t rgb);
	void setAllLedsRGB(uint16_t red, uint16_t green, uint16_t blue);
	void setAllLed(uint16_t gsvalue);
	void setLedAppend(uint16_t led_number, uint16_t red, uint16_t green, uint16_t blue);
	void setChannel(uint16_t channel_number, uint16_t value);

	//-------------Control Mode Parameters------------------------------------------------------------------------
	void setLedDotCorrection(uint16_t led_number, uint8_t color_channel_index, uint8_t dc_value);
	void setMaxCurrent(uint8_t red, uint8_t green, uint8_t blue);
	void setMaxCurrent(uint8_t MCRGB);
	void setBrightnessCurrent(uint8_t rgb);
	void setBrightnessCurrent(uint8_t red, uint8_t green, uint8_t blue);
	void setFunctionControlData(bool DSPRPT, bool TMGRST, bool RFRESH, bool ESPWM, bool LSDVLT);
	void setRGBPinOrder(uint8_t red_pos, uint8_t green_pos, uint8_t blue_pos);
	void setSinglePinOrder(uint16_t led_number, uint8_t color_channel_index, uint8_t position);
	void setSingleRGBPinOrder(uint16_t led_number, uint8_t red_pos, uint8_t green_pos, uint8_t blue_pos);


	static const uint8_t tlc_count; // This
	static const uint8_t COLOR_CHANNEL_COUNT = 3;
	static const uint8_t LEDS_PER_CHIP = 16;
	static bool force_max_current;
	static float max_current_amps;

	static uint8_t dot_correction_data[][LEDS_PER_CHIP][COLOR_CHANNEL_COUNT];
	static uint8_t rgb_order[][LEDS_PER_CHIP][COLOR_CHANNEL_COUNT];
	static uint16_t grayscale_data[][LEDS_PER_CHIP][COLOR_CHANNEL_COUNT];

	uint8_t rgb_order_default[3] = {0, 1, 2};

private:
	int debug = 0;
	SPIMUX *_spimux;
	uint8_t _gslat;
	uint8_t _spi_mosi;
	uint8_t _spi_clk;

	uint8_t _function_data;
	uint16_t _bright_red = 0;
	uint16_t _bright_green = 0;
	uint16_t _bright_blue = 0;
	uint8_t _Max_Current_Red = 0;
	uint8_t _Max_Current_Green = 0;
	uint8_t _Max_Current_Blue = 0;

	/* SPI */
	uint8_t _spi_buffer;
	int8_t _spi_buffer_size = 7;
};



#endif /* TLC5955_HPP_ */
