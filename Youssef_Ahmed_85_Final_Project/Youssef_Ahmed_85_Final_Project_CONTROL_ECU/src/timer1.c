/********************************************************************************
 *																				*
 *				\brief:	Timer/Counter1 Driver	-	Source file					*
 *																				*
********************************************************************************/

#include <timer1.h>

/********************************************************************************
 *					\note:	Section #1:	Global Variables						*
********************************************************************************/

/* Represents callback(s) for Timer1 events, including both normal and CTC modes */
static Timer1_CallbackType Timer1_callbacks;



/* Represents time in seconds  */
uint8 timer1_counter;

uint8 g_Timer1_mode_of_operation;

/********************************************************************************
 *					\note:	Section #2:	Interrupt Service Routines (ISRs)		*
********************************************************************************/

/* ISR that will execute when "Timer/Counter1 Overflow" interrupt occurs */
ISR(TIMER1_OVF_vect)
{
	/*
	 *	In normal operation, the Timer/Counter Overflow Flag (TOV1) will
	 *	be set in the same timer clock cycle as the TCNT1 becomes zero.
	 */

	/* Checks if the global pointer g_NormalcallBackPtr is not pointing to NULL. */
	if(Timer1_callbacks.g_NormalcallBackPtr != NULL_PTR)
	{
		/* Call the callback function using pointer to function g_NormalcallBackPtr(); */
		Timer1_callbacks.g_NormalcallBackPtr();
	}
}

/* ISR that will execute when "Timer/Counter1 Compare Match A" interrupt occurs */
ISR(TIMER1_COMPA_vect)
{
	/*
	 *	An interrupt can be generated at each time the counter value reaches the
	 *	TOP value by using the OCF1A to define the TOP value.
	 */

	/* Checks if the global pointer g_CTCcallBackPtr is not pointing to NULL. */
	if(Timer1_callbacks.g_CTCcallBackPtr != NULL_PTR)
	{
		/* Call the callback function using pointer to function g_CTCcallBackPtr(); */
		Timer1_callbacks.g_CTCcallBackPtr();
	}
}

/********************************************************************************
 *					\note:	Section #3:	Timer1 Driver Functions					*
********************************************************************************/

/**
 *	\see	:	TIMER1_vStartTimerNormal function prototype in timer1.h.
 */
void TIMER1_vStartTimerNormal(Timer1_intervalType interval_in_seconds)
{
	/* Configuration struct members:		  {mode , prescaler, initial,  compare} */
	Timer1_ConfigType timer1_configurations = {NORMAL, F_CPU_256, PRELOAD, NOT_CTC};

	/* Pass all desired configurations to the Timer1_init function */
	Timer1_init(&timer1_configurations);

	/* Start the timer counting decrementally from the given time interval till zero */
	timer1_counter = interval_in_seconds;

	/* Poll until reaching zero = required interval_in_seconds achieved */
	while(timer1_counter != ZERO);
}

/**
 *	\see	:	TIMER1_vStartTimerCTC function prototype in timer1.h.
 */
void TIMER1_vStartTimerCTC(Timer1_intervalType interval_in_seconds)
{
	/* Configuration struct members:		  {mode , prescaler, initial,  compare} */
	Timer1_ConfigType timer1_configurations = {CTC, F_CPU_256, BOTTOM, COMPARE};

	/* Pass all desired configurations to the Timer1_init function */
	Timer1_init(&timer1_configurations);

	/* Start the timer counting decrementally from the given time interval till zero */
	timer1_counter = interval_in_seconds;

	/* Poll until reaching zero = required interval_in_seconds achieved */
	while(timer1_counter != ZERO);
}

void TIMER1_vStartTimer(Timer1_intervalType interval_in_seconds)
{
	switch(g_Timer1_mode_of_operation)
	{
		case NORMAL	:
		{
			TIMER1_vStartTimerNormal(interval_in_seconds);
			break;
		}
		case CTC	:
		{
			TIMER1_vStartTimerCTC(interval_in_seconds);
			break;
		}
	}
}

/**
 *	\see	:	Timer1_init function prototype in timer1.h.
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	/* Set timer1 initial value to required initial_value */
	TCNT1 = (uint16)(Config_Ptr -> initial_value);

	/* Check the desired mode of operation */
	switch(Config_Ptr -> mode)
	{
		case NORMAL:
		{
			/* Choose normal Mode WGM11=0 WGM10=0 (Mode Number 0) */
			TCCR1A = (TCCR1A & TCCR1A_NON_WGM_BITS) | (Config_Ptr -> mode);
			/* Enable the overflow interrupts */
			SET_BIT(TIMSK, TOIE1);

			break;
		}

		case CTC:
		{
			/*
			 *	1.	As a non-PWM mode, COM1A1=1 COM1A0=0 will clear OC1A on compare match.
			 *	2.	FOC1A and FOC1B are only active when the WGM13:0 bits specifies a non-PWM mode.
			 *	3.	Choose CTC Mode WGM12=1 WGM13=0 (Mode Number 4). Here (0x04 << 1) = 0000 1000.
			 */
			TCCR1A = (ENABLE << FOC1B) | (ENABLE << FOC1A) | (ENABLE << COM1A1) ;
			TCCR1B = (TCCR1B & TCCR1B_NON_WGM_BITS) | ((Config_Ptr -> mode) << ONE_BIT);

			/* Set the compare value to required compare_value */
			OCR1A = (uint16)(Config_Ptr -> compare_value);

			/* Set PD5/OC1A pin as output */
			GPIO_setupPinDirection(PORTD_ID, PIN5_ID, PIN_OUTPUT);

			/* Enable Timer1 Compare A Interrupt */
			SET_BIT(TIMSK, OCIE1A);

			break;
		}
	}

	/* Choose the pre-scaler (CS bits) */
	TCCR1B = (TCCR1B & TCCR1B_NON_CS_BITS) | (Config_Ptr -> prescaler);
}

/**
 *	\see	:	Timer1_deInit function prototype in timer1.h.
 */
void Timer1_deInit(void)
{
	/* Disable Timer1 interrupt*/
    TIMSK = 0;

    /* Stop Timer1 by setting clock source to "No clock source */
    TCCR1B = 0;

    /* Reset Timer1 count value */
    TCNT1 = 0;

    /* Reset Timer1 control registers to default values */
    TCCR1A = 0;
    TCCR1B = 0;

    /* Reset Timer1 output compare registers */
    OCR1A = 0;
    OCR1B = 0;
}

/**
 *	\see	:	Timer1_setNormalCallBack function prototype in timer1.h.
 */
void Timer1_setCallBack(const Timer1_CallbackType* TimerCallback)
{
	/*
	 * Assigns the normal callback function pointer from the provided TimerCallback
	 * structure to the g_NormalcallBackPtr member of the Timer1_callbacks structure.
	 */
	Timer1_callbacks.g_NormalcallBackPtr = TimerCallback -> g_NormalcallBackPtr;

	/*
	 * Assigns the CTC callback function pointer from the provided TimerCallback
	 * structure to the g_CTCcallBackPtr member of the Timer1_callbacks structure.
	 */
	Timer1_callbacks.g_CTCcallBackPtr = TimerCallback -> g_CTCcallBackPtr;
}
