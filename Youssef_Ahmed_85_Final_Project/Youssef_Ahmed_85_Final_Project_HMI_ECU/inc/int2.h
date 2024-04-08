/*
 * int2.h
 *
 *  Created on: Apr 1, 2024
 *      Author: Youssef
 */

#ifndef INC_INT2_H_
#define INC_INT2_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <std_types.h>
#include <common_macros.h>

void INT2_init(void);

void INT2_setCallBack(void(*a_ptr)(void));
#endif /* INC_INT2_H_ */
