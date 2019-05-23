/*
 * STM32F411USART2.hpp
 *
 *  Created on: Apr 24, 2019
 *      Author: rev
 */

#ifndef STM32F411USART2_HPP_
#define STM32F411USART2_HPP_

#include "stm32f4xx.h"
#include "buffer.hpp"

extern "C" void USART2_IRQHandler(void);

class STM32F411USART2 {
	friend void USART2_IRQHandler(void);
	bool isTransmitting = false;
	Buffer<uint8_t, 1024> rxBuffer;
	Buffer<uint8_t, 1024> txBuffer;
	STM32F411USART2();
	static STM32F411USART2 * instance;
public:
	/**
	 * Méthode static du singleton qui retourne l'instance de l'objet si elle est libre.
	 * @return Retourne l'instance de l'objet
	 */
	static STM32F411USART2* getInstance();
	/**
	 * Destructeur
	 */
	virtual ~STM32F411USART2();
	/**
	 * Envoie un byte dans le port de communication
	 * @param data à envoyer
	 */
	void write(uint8_t data);
	/**
	 * Envoie une série de Bytes dans le port de communication
	 * @param data Tableau à envoyer
	 * @param nBytes nombre de byte à envoyer
	 */
	void sendBytes(uint8_t *data, uint32_t nBytes);
	/**
	 * Envoie la valeur décimal de la variable vers le port de communication
	 * @param byte Variable à envoyer
	 */
	void sendByteToString(uint32_t byte);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl à envoyer
	 * Ex: 2 ---> 00000010
	 */
	void sendByte8ToBinaryString(uint8_t data);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl à envoyer
	 * Ex: 2 ---> 0000000000000010
	 */
	void sendByte16ToBinaryString(uint16_t data);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl à envoyer
	 * Ex: 2 ---> 00000000000000000000000000000010
	 */
	void sendByte32ToBinaryString(uint32_t data);
	/**
	 * Envoie un chaine de caractère terminé par un Null vers le port de communication
	 * @param s La chainne de caractères
	 */
	void sendString(const char *s);
	/**
	 * Envoie un chaine de caractère terminé par un Null vers le port de communication
	 * @param s La chainne de caractères
	 */
	void sendString(uint8_t *u);
	/**
	 * Lie la donné reçu dans le port de communication
	 * @return
	 */
	uint8_t read();
	/**
	 * Retourne vrai lorsu'un donnée est disponible dans le port de communication
	 * @return 1 lorsqu'un donnée est disponible.
	 */
	bool dataAvailable();
	/**
	 * Configure la vitesse du port de communication
	 * @param baudrate
	 */
	/**
	 * Ajuste la vitesse de communication du port
	 * @param baudrate
	 */
	void setBaudRate(uint32_t baudrate);
	/**
	 * Active l'écho de réception pour fin de déverminage
	 * @param state 1 = on, 0 = off
	 */
	void setEcho(bool state);
};

#endif /* STM32F411USART2_HPP_ */

