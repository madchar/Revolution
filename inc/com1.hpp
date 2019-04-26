/*
 * com.hpp
 *
 *  Created on: Mar 27, 2019
 *      Author: rev
 */

#ifndef COM1_HPP_
#define COM1_HPP_

#include "stm32f4xx_usart.h"
#include "hardware.h"
#include "buffer.hpp"
#include "flash.hpp"

extern "C" {
void USART2_IRQHandler(void);
}

class Com1 {
public:

	static Com1* getInstance();
	virtual ~Com1();
	void write(uint8_t data);
	void sendBytes(uint8_t *data, uint32_t nBytes);
	void sendByteToString(uint8_t byte);
	void sendbyteToString(uint16_t byte);
	void sendByte8ToBinaryString(uint8_t data);
	void sendByte16ToBinaryString(uint16_t data);
	void sendByte32ToBinaryString(uint32_t data);
	void sendString(const char *s);
	void sendString(uint8_t *u);
	uint8_t read();
	void incommingDataDecoder(Flash* flash);
	bool dataAvailable();
	void setBaudRate(uint32_t baudrate);
	void setEcho(bool state);
	void parseTram();

private:

	Com1();
	bool echo;
	bool isTransmitting;
	Buffer<uint8_t, 4096> rxBuffer;
	Buffer<uint8_t, 4096> txBuffer;
	static Com1* instance;
	friend void USART1_IRQHandler(void);

	enum COMM_STATE {
		WAIT, RXPAYLOAD, VALIDATE, FILE_TRANSFER
	};
	COMM_STATE commState;
	uint16_t commRxCnt;
	static constexpr uint8_t TRAM_SIZE = 6;
	char tram[TRAM_SIZE + 2];
	uint8_t pixelColumnBuffer[1156];
	uint16_t pixelColumnBufferCntr;
	bool timeout = 0;
	uint8_t retry = 0;

	const char CMD_ImageReadyToTransfer[7] = { "imgrdy" };

};

#endif /* COM1_HPP_ */
