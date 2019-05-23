/*
 * com.hpp
 *
 *  Created on: Mar 27, 2019
 *      Author: rev
 */

#ifndef STM32F411USART1_HPP_
#define STM32F411USART1_HPP_

#include "stm32f4xx_usart.h"
#include "hardware.h"
#include "buffer.hpp"
#include "flash.hpp"
#include "stm32f4timer.hpp"

extern "C" void TIM3_IRQHandler(void);
extern "C" void USART1_IRQHandler(void);

class STM32F411USART1 {
public:
	/**
	 * M�thode static du singleton qui retourne l'instance de l'objet si elle est libre.
	 * @return Retourne l'instance de l'objet
	 */
	static STM32F411USART1* getInstance();
	/**
	 * Destructeur
	 */
	virtual ~STM32F411USART1();
	/**
	 * Envoie un byte dans le port de communication
	 * @param data � envoyer
	 */
	void write(uint8_t data);
	/**
	 * Envoie une s�rie de Bytes dans le port de communication
	 * @param data Tableau � envoyer
	 * @param nBytes nombre de byte � envoyer
	 */
	void sendBytes(uint8_t *data, uint32_t nBytes);
	/**
	 * Envoie la valeur d�cimal de la variable vers le port de communication
	 * @param byte Variable � envoyer
	 */
	void sendByteToString(uint32_t byte);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl � envoyer
	 * Ex: 2 ---> 00000010
	 */
	void sendByte8ToBinaryString(uint8_t data);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl � envoyer
	 * Ex: 2 ---> 0000000000000010
	 */
	void sendByte16ToBinaryString(uint16_t data);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl � envoyer
	 * Ex: 2 ---> 00000000000000000000000000000010
	 */
	void sendByte32ToBinaryString(uint32_t data);
	/**
	 * Envoie un chaine de caract�re termin� par un Null vers le port de communication
	 * @param s La chainne de caract�res
	 */
	void sendString(const char *s);
	/**
	 * Envoie un chaine de caract�re termin� par un Null vers le port de communication
	 * @param s La chainne de caract�res
	 */
	void sendString(uint8_t *u);
	/**
	 * Lie la donn� re�u dans le port de communication
	 * @return
	 */
	uint8_t read();
	/**
	 * Cette m�thode surveille le port de communication entre le seveur et le R�volution
	 * @param flash pointeur de la flash pour pouvoir y acc�der directement
	 * Cette m�thode surveille et filtre les trams de communication entrantent du serveur.
	 * Elle s'occupe �galement du transf�re d'images entre le serveur et le R�volution
	 */
	void incommingDataDecoder(Flash* flash);
	/**
	 * Retourne vrai lorsu'un donn�e est disponible dans le port de communication
	 * @return 1 lorsqu'un donn�e est disponible.
	 */
	bool dataAvailable();
	/**
	 * Ajuste la vitesse de communication du port
	 * @param baudrate
	 */
	void setBaudRate(uint32_t baudrate);
	/**
	 * Active l'�cho de r�ception pour fin de d�verminage
	 * @param state 1 = on, 0 = off
	 */
	void setEcho(bool state);
	/**
	 * D�code la trame entrante
	 * @param flash pointeur de flahs pour acc�der � la m�moire directement
	 */
	void parseTram(Flash *flash);
	/**
	 * G�naire et envoie la liste de fichier pr�sent dans le carrousel vers le serveur
	 * @param flash Pointeru de flash pour acc�der directement la m�moire
	 */
	void sendFilenameList(Flash *flash);

	bool isReadyToTransfer;
	bool isOkToTransfer;
private:

	STM32F411USART1();
	bool echo;
	bool isTransmitting;
	Buffer<uint8_t, 1024> rxBuffer;
	Buffer<uint8_t, 1024> txBuffer;
	static STM32F411USART1* instance;

	friend void USART1_IRQHandler(void);
	friend void TIM3_IRQHandler(void);

	/**
	 * �num�ration d'�tat pour la lecture de trames
	 */
	enum parseTram_e {
		WAIT,     //!< WAIT
		RXPAYLOAD,     //!< RXPAYLOAD
		VALIDATE  //!< VALIDATE
	} parseRxTram = WAIT;
	/**
	 * �num�ration d'�tat pour les �tapes de tranf�res
	 */
	enum commState_e {
		IDLE,               //!< IDLE
		WAIT_OK_TO_TRANSFER,               //!< WAIT_OK_TO_TRANSFER
		ASK_FILE_TO_SERVER, //!< ASK_FILE_TO_SERVER
		SAVE_FILE,          //!< SAVE_FILE
		SAVE_FILENAME,      //!< SAVE_FILENAME
		TRANSFER_COMPLETED, //!< TRANSFER_COMPLETED
		TRANSFER_FAILED     //!< TRANSFER_FAILED
	} commState = IDLE;

	uint16_t commRxCnt = 0;
	uint8_t cmdCntr = 0;
	static constexpr uint8_t TRAM_SIZE = 6;
	char tram[TRAM_SIZE + 2];

	uint16_t rxColumnNo = 0;
	uint16_t rxImageNo = 0;
	uint8_t pixelColumnBuffer[1156];
	uint16_t pixelColumnBufferCntr;
	uint8_t filename[256];

	bool timeout = 0;
	uint8_t timeoutCntr = 0;
	uint8_t retry = 0;
	static constexpr uint8_t TIME_OUT = 2;
	static constexpr uint8_t MAX_RETRY = 3;

	const char CMD_ImageQuantity[7] = { "imgqty" };
	const char CMD_ImageReadyToTransfer[7] = { "imgrdy" };
	const char CMD_GetFilenameList[7] = { "getlst" };
	const char CMD_Filename[7] = { "lfname" };
	const char CMD_Red[4] = { "red" };
	const char CMD_Green[4] = { "grn" };
	const char CMD_Blue[4] = { "blu" };
	const char CMD_GlobalBrightness[4] = { "glb" };
	const char CMD_Del0[7] = { "del000" };
	const char CMD_Del1[7] = { "del001" };
	const char CMD_Del2[7] = { "del002" };
	const char CMD_Del3[7] = { "del003" };
	const char CMD_Del4[7] = { "del004" };
	const char CMD_Del5[7] = { "del005" };
	const char CMD_Del6[7] = { "del006" };
	const char CMD_Del7[7] = { "del007" };
	const char CMD_Del8[7] = { "del008" };
	const char CMD_Del9[7] = { "del009" };
	const char CMD_Del10[7] = { "del010" };
	const char CMD_Del11[7] = { "del011" };
	const char CMD_Del12[7] = { "del012" };
	const char CMD_Del13[7] = { "del013" };
	const char CMD_Del14[7] = { "del014" };

	//STM32F4Timer *commTimeOut;


};
#endif /* STM32F411USART1_HPP_ */
