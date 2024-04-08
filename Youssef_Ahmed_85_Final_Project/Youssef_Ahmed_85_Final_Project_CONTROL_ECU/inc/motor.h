/*
 * motor.h
 *
 *  Created on: Mar 8, 2024
 *      Author: Youssef
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include <std_types.h>
#include <gpio.h>
#include <pwm.h>
#include <common_macros.h>

#define MOTOR_EN1_PORT	PORTB_ID
#define MOTOR_EN1_PIN	PIN3_ID

#define MOTOR_IN1_PORT	PORTB_ID
#define MOTOR_IN1_PIN	PIN4_ID

#define MOTOR_IN2_PORT	PORTB_ID
#define MOTOR_IN2_PIN	PIN5_ID


typedef enum
{
	MOTOR_DIRECTION_OFF	,
	MOTOR_DIRECTION_CW	,
	MOTOR_DIRECTION_ACW
}	DcMotor_DirectionType;

typedef enum
{
	MOTOR_SPEED_OFF	= 0,
	MOTOR_SPEED_25 	= 25,
	MOTOR_SPEED_50 	= 50,
	MOTOR_SPEED_75 	= 75,
	MOTOR_SPEED_100	= 100
}	DcMotor_SpeedType;

void DcMotor_Init(void);

void DcMotor_Rotate(DcMotor_DirectionType state,DcMotor_SpeedType speed);

#endif /* INC_MOTOR_H_ */
