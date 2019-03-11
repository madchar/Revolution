/*
 * tlc5955.cpp
 *
 *  Created on: Mar 1, 2019
 *      Author: Zandwich
 */

#include "tlc5955.hpp"

TLC5955::TLC5955(SPIMUX* spimux)
{
	_spimux = spimux;

}

TLC5955::~TLC5955() {
}

void TLC5955::init() {
}

void TLC5955::updateControl(SPIMUX* spimux) {
}

void TLC5955::updateLeds(SPIMUX* spimux) {
}

void TLC5955::latch() {
}

void TLC5955::setBuffer(uint8_t bit) {
}

void TLC5955::setControlModeBit(bool isControlMode) {
}

void TLC5955::flushBuffer() {
}

void TLC5955::setLedRGB(uint16_t led_number, uint16_t red, uint16_t green,uint16_t blue) {
}

void TLC5955::setLedRGB(uint16_t led_number, uint16_t rgb) {
}

void TLC5955::setAllLedsRGB(uint16_t red, uint16_t green, uint16_t blue) {
}

void TLC5955::setLedAppend(uint16_t led_number, uint16_t red, uint16_t green,
		uint16_t blue) {
}

void TLC5955::setChannel(uint16_t channel_number, uint16_t value) {
}

void TLC5955::setLedDotCorrection(uint16_t led_number,
		uint8_t color_channel_index, uint8_t dc_value) {
}

void TLC5955::setMaxCurrent(uint8_t red, uint8_t green, uint8_t blue) {
}

void TLC5955::setMaxCurrent(uint8_t rgb) {
}

void TLC5955::setBrightnessCurrent(uint8_t rgb) {
}

void TLC5955::setBrightnessCurrent(uint8_t red, uint8_t green, uint8_t blue) {
}

void TLC5955::setFunctionControlData(bool DSPRPT, bool TMGRST, bool RFRESH,
		bool ESPWM, bool LSDVLT) {
}

void TLC5955::setRGBPinOrder(uint8_t red_pos, uint8_t green_pos,
		uint8_t blue_pos) {
}

void TLC5955::setSinglePinOrder(uint16_t channel, uint8_t color_channel_index,
		uint8_t position) {
}

void TLC5955::setSingleRGBPinOrder(uint16_t channel, uint8_t red_pos,
		uint8_t green_pos, uint8_t blue_pos) {
}
