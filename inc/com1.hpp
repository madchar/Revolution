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

class Com1
{
public:

	static Com1* getInstance();
	virtual ~Com1();
	void write(uint8_t data);
	void sendBytes(uint8_t *data, uint32_t nBytes);
	void sendByte8ToBinaryString(uint8_t data);
	void sendByte16ToBinaryString(uint16_t data);
	void sendByte32ToBinaryString(uint32_t data);
	void sendString(const char *s);
	void sendString(uint8_t *u);
	uint8_t read();
	void incommingDataDecoder(Flash* flash);
	bool dataAvailable();
	void setBaudeRate(uint32_t baudrate);
	void setEcho(bool state);
	void resetTram();


private:

	Com1();
	bool 						binaryTransferMode;
	uint8_t						tramPosition;
	bool 						tramSynched;
	char 						tram[16];
	static constexpr uint8_t	tramSize = 16;
	uint8_t						pixelColumnBuffer[1156];
	uint16_t					pixelColumnBufferCntr;
	bool 						echo;
	bool 						isTransmitting;
	uint8_t 					pixelColumn[1156];
	Buffer<uint8_t, 4096>	 	rxBuffer;
	Buffer<uint8_t, 4096> 		txBuffer;
	static Com1*				instance;
	friend void USART2_IRQHandler(void);

	enum comMode_e {LISTENING, SORT_COMMAND, BINARY_TRANSFER};
	comMode_e comMode;

	const char				CMD_ImageReadyToTransfer[7]={"imgrdy"};


};


#endif /* COM1_HPP_ */
