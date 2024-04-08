#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include <avr/io.h>
#include <common_macros.h>
#include <gpio.h>

#define	BUZZER_PORT	PORTB_ID
#define BUZZER_PIN	PIN6_ID


void Buzzer_init(void);


void Buzzer_off(void);


void Buzzer_on(void);

#endif /* INC_BUZZER_H_ */
