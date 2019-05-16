/*
 * stm32f4timer.hpp
 *
 *  Created on: Apr 5, 2019
 *      Author: Matthieu Madran
 */

#ifndef STM32F4TIMER_HPP_
#define STM32F4TIMER_HPP_
#include "stm32f4xx_tim.h"
#include "stm32f4xx.h"


class STM32F4Timer {

public:
	/**
	 * Constructeur du timer
	 * @param Le timer (Exemple TIM2 pour le timer2)
	 * @param La pÃ©riode du timer en us (de 2 Ã  65535000 us)
	 * PWM_frequency = timer_tick_frequency / (TIM_Period + 1) ** 1MHz**
	 * timer_tick_frequency = Timer_default_frequency / (prescaler_set + 1)
	 */
	STM32F4Timer(TIM_TypeDef *tmr,uint32_t frequency, uint16_t prescaler,bool interruptEnable);
	~STM32F4Timer() {}

	/**
	 * Réglage du timer en mode PWM
	 * @param ch Channel
	 * @param dutyCycle
	 */
	void enablePWM(uint8_t ch, uint32_t dutyCycle);
	/**
	 * Activation de l'interrupt
	 */
	void enableITUpdate();
	/**
	 * Démarrage du timer
	 */
	void startTimer();
	/**
	 * Arrêt du timer
	 */
	void stopTimer();

private:
	TIM_TypeDef * timer;
	uint32_t timerTickFreq = 50000000; // 50MHz
	uint32_t timerPeriod = 1000;
};




#endif /* STM32F4TIMER_HPP_ */
