/*
 * pwm.h
 *
 *  Created on: Mar 8, 2024
 *      Author: Youssef
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include <avr/io.h>
#include <gpio.h>
#include <common_macros.h>

#define MOTOR_EN1_PORT	PORTB_ID
#define MOTOR_EN1_PIN	PIN3_ID

#define ORIGINAL_MAX_RANGE	255
#define DESIRED_MAX_RANGE	100.0

#define SCALER_TO_255	(ORIGINAL_MAX_RANGE/DESIRED_MAX_RANGE)

void Timer0_PWM_Init(uint8 set_duty_cycle);

#endif /* INC_PWM_H_ */
