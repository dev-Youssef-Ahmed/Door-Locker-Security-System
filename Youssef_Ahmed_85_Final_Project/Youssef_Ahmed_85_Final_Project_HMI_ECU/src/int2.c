/*
 * int2.c
 *
 *  Created on: Apr 1, 2024
 *      Author: Youssef
 */


#include <int2.h>

/* Holds the address of the callback function */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

ISR(INT2_vect)
{
	/*
	 *  A USART Receive Complete Interrupt will be generated only if the RXCIE
	 *  bit is written to one, the Global Interrupt Flag in SREG is written to
	 *  one and the RXC bit in UCSRA is set.
	 */

	/* Checks if the global pointer g_callBackPtr is not pointing to NULL. */
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the callback function using pointer to function g_callBackPtr(); */
		(*g_callBackPtr)();
	}
}

void INT2_setCallBack(void(*a_ptr)(void))
{

	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

void INT2_init(void)
{
	/* Port B Data Direction Register – DDRB */
	/* Set the RESUME button as input - PB2 */
	CLEAR_BIT(DDRB,PB2);

	/* Port B Data Register – PORTB */
	/* Activate the internal pull-up resistor - PB2 */
	SET_BIT(PORTB,PB2);

	/*MCU Control and Status Register – MCUCSR*/
	/* Interrupt Sense Control 2 - ISC2: CLR for Falling edge */
	SET_BIT(MCUCSR,ISC2);

	/* General Interrupt Control Register – GICR */
	/* External Interrupt Request 0 Enable - INT2 */
	SET_BIT(GICR,INT2);

	/* General Interrupt Flag Register – GIFR */
	/* External Interrupt Request 0 Enable - INTF2 */
	/* SET_BIT(GIFR,INTF2); */
}
