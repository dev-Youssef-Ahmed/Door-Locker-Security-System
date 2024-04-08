/********************************************************************************
 *																				*
 *				\brief:	Timer/Counter1 Driver	-	Header file					*
 *																				*
********************************************************************************/

/*
 * 	Header Guard:	It's a preprocessor directive used to prevent a header file
 * 	from being included multiple times in the same compilation unit, which could
 * 	lead to issues like redefinition errors.
 */
#ifndef INC_TIMER1_H_
#define INC_TIMER1_H_

#include <avr/io.h>			/* Contains AVR registers and bits macros */
#include <avr/interrupt.h>	/* Manages AVR interrupts and defining (ISRs) */
#include <gpio.h>			/* The portable DIO driver to manage I/O ports */
#include <common_macros.h>	/* Contains bit manipulation macros */
#include <std_types.h>		/* Contains the standard data types */
#include <application.h>

/********************************************************************************
 *			\note:	Section #1:	Macros & enums of Timer1						*
********************************************************************************/

/*
 * An enum to replace magic numbers will be used as intervals,
 * update it with new time interval in seconds if needed
 */
typedef enum
{
	ONE_SECOND		= 1	,
	TWO_SECONDS		= 2	,
	THREE_SECONDS	= 3	,
	TEN_SECONDS		= 10,
	FIFTEEN_SECONDS = 15,
	SIXTY_SECONDS	= 60,
} Timer1_intervalType;

/* Each macro is meaningful as its name */
#define NORMAL_MODE_IN_WGM	0
#define CTC_MODE_IN_WGM		4
#define TCCR1A_NON_WGM_BITS	0xFC
#define TCCR1B_NON_WGM_BITS	0xE7
#define TCCR1B_NON_CS_BITS	0xF8
#define ONE_BIT				1		/* To avoid magic numbers */

/* Extern this global variable to be used in application.c */
extern uint8 timer1_counter;
extern uint8 g_Timer1_mode_of_operation;

/* Using function pointers, such as g_(Normal/CTC)callBackPtr_t, allows for dynamic behavior in programs.  */
typedef void (*g_NormalcallBackPtr_t)(void);
typedef void (*g_CTCcallBackPtr_t)(void);

/*
 * Stores callback functions for Timer1 events, with separate pointers for normal
 * and CTC operations, enabling flexibility in event handling and customization.
 */
typedef struct
{
	g_NormalcallBackPtr_t	g_NormalcallBackPtr;
	g_CTCcallBackPtr_t		g_CTCcallBackPtr;
} Timer1_CallbackType;

#define TIMER1_BOTTOM	0x0000	/* The minimum value for 16-bit timer */
#define TIMER1_MAX		0xFFFF	/* The maximum value for 16-bit timer */
#define TIMER1_TOP		0x7A12	/* Tick per secnod of F_CBU = 8Mhz with /256 prescaler */

/* Defines the possible initial values */
typedef enum
{
	BOTTOM	= TIMER1_BOTTOM,			/* Used for CTC mode */
	PRELOAD	= TIMER1_MAX - TIMER1_TOP	/* Used for Normal mode */
} Timer1_InitialValueType;

/* Defines the possible compare values */
typedef enum
{
	NOT_CTC	= TIMER1_BOTTOM,			/* Used for Normal mode */
	COMPARE	= TIMER1_TOP				/* Used for CTC mode */
} Timer1_CompareValueType;

/* All pre-scalers of Timer1 (in order) */
typedef enum
{
	NO_CLOCK	,
	F_CPU_CLOCK	,
	F_CPU_8		,
	F_CPU_64	,
	F_CPU_256	,
	F_CPU_1024
} Timer1_PrescalerType;

/* All modes of Timer1 (in order), only Normal & CTC modes are supported now */
typedef enum
{
	NORMAL	= NORMAL_MODE_IN_WGM	,
/*	PWM_8bit						,
	PWM_9bit						,
	PWM_10bit						,*/
	CTC		= CTC_MODE_IN_WGM		,
/*	fastPWM_8bit					,
	fastPWM_9bit					,
	fastPWM_10bit					,
	PWM_phase_and_freq				,
	PWM_phase_and_freq1				,
	PWM_pahse						,
	PWM_pahse1						,
	CTC1							,
	RESERVED						,
	fastPWM							,
	fastPWM1*/
} Timer1_ModeType;

 /* The configuration structure of Timer1, will be passed to the Timer1_init function */
typedef struct
{
	Timer1_ModeType			mode			;
	Timer1_PrescalerType	prescaler		;
	Timer1_InitialValueType	initial_value	;
	Timer1_CompareValueType	compare_value	;
} Timer1_ConfigType;

/********************************************************************************
 *			\note:	Section #3:	Timer1 Driver Functions Prototypes				*
********************************************************************************/

/**
 *	\brief	:	The end user timer fuc=nction that count time in seconds with a given mode
 *	\param 1:	One of predefined modes at "\see: Timer1_ModeType".
 *	\param 2:	One of predefined values at "\see: Timer1_intervalType".
 *	\return	:	Void
 **/
void TIMER1_vStartTimer(Timer1_intervalType interval_in_seconds);
/**
 *	\brief	:	A function to calculate the time if normal mode used
 *	\param	:	One of predefined values at "\see: Timer1_intervalType".
 *	\return	:	Void
 **/
void TIMER1_vStartTimerNormal(Timer1_intervalType interval);

/**
 *	\brief	:	A function to calculate the time if normal mode used
 *	\param	:	One of predefined values at "\see: Timer1_intervalType".
 *	\return	:	Void
 **/
void TIMER1_vStartTimerCTC(Timer1_intervalType interval);

/**
 *	\brief	:	Initiates the configurable Timer1 driver.
 *	\param	:	Configurations structure of "\see: Timer1_ConfigType".
 *	\return	:	Void
 **/
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/**
 *	\brief	:	Disables Timer1 driver.
 *	\param	:	Void.
 *	\return	:	Void.
 **/
void Timer1_deInit(void);

/**
 *	\brief	:	This function assigns callback pointers from the provided TimerCallback
 *				structure to the corresponding members of the Timer1_callbacks structure.
 *	\param	:	Configurations structure of "\see: Timer1_CallbackType".
 *	\return	:	Void.
 **/
void Timer1_setCallBack(const Timer1_CallbackType* TimerCallback);

#endif /* INC_TIMER1_H_ */
