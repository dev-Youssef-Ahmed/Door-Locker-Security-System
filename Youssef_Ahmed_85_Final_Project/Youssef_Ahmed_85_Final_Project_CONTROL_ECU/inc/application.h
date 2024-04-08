/************************ PREPROCESSOR HEADER GUARD ***************************/
/*
 * 	A preprocessor directive used to prevent a header file from being included
 * 	multiple times in the same compilation unit, which could lead to issues like
 * 	redefinition errors.
 */
#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

/************************** HEADER FILES INCLUDES *****************************/

/* Used to import the externed g_current_control_state */
#include <main.h>

/* Used to use TIMER1 functions */
#include <timer1.h>

/* Used to use GPIO functions */
#include <gpio.h>

/* Used to use UART functions */
#include <uart.h>

/* Used to use INT2 functions */
#include <int2.h>

/* Used to use BUZZER functions */
#include <buzzer.h>

/* Used to use DC MOTOR functions */
#include <motor.h>

/* Used to use EEPROM functions */
#include <external_eeprom.h>

/* Used to use standrad datatypes (boolean, uin8, ..) */
#include <std_types.h>

/********************************* ENUMS **************************************/

/**
 *	\note 1	:	Generally, enums are used to avoid using magic numbers in the
 * 				code, and to specify a range for each customized datatype, which
 * 				if been exceeded, the AVR-GCC toolchain will warn you.
 *
 * 	\note 2	:	Both of HMI_ECU and CONTROL_ECU uses the same necessary enums.
 **/

/* ========================================================================== */
/**
 *	\brief	:	Contains the IDs of control byte of both TX and RX processes.
 *	\see	:	All UART application functions.
 **/
typedef enum
{
	CTRL_ERROR					,
	CTRL_SUCCESS				,
	CTRL_GET_NEW_PASSWORD		,
	CTRL_CONFIRM_NEW_PASSWORD	,
	CTRL_PASSWORD_MATCHED		,
	CTRL_PASSWORD_MISMATCHED	,
	CTRL_PASSWORD_SAVED			,
	CTRL_ENTER_PASSWORD			,
	CTRL_MAIN_MENU				,
	CTRL_DOOR_OPEN				,
	CTRL_DOOR_HOLD				,
	CTRL_DOOR_CLOSE				,
	CTRL_WRONG_PASSWORD			,
	CTRL_LOCK_SYSTEM			,
	CTRL_EMPTY			= 0xEE	,
} UART_ControlByteType;

/* ========================================================================== */
/**
 *	\brief	:	Contains the IDs of byte of both TX and RX frames.
 *	\see	:	All UART application functions.
 *
 **/
typedef enum
{
	BYTE_0_CONTROL		,
	BYTE_1_PASS0_OR_CMD	,
	BYTE_2_PASS1		,
	BYTE_3_PASS2		,
	BYTE_4_PASS3
} frameType;

/* ========================================================================== */
/**
 *	\brief	:	Contains the IDs valid user commands.
 *	\see	:	APP_u8GetCommandFromUser.
 *
 **/
typedef enum
{
	CMD_OPN_DOOR	= '+',
	CMD_CHNG_PSWRD	= '-'
} userCommandsType;

/* ========================================================================== */
/**
 *	\brief	:	General enum for numbers (0-9) to avoid magic numbers.
 *	\see	:	All application functions.
 **/
enum
{
	ZERO	,
	ONE		,
	TWO		,
	THREE	,
	FOUR	,
	FIVE	,
	SIX		,
	SEVEN	,
	EIGHT	,
	NINE	,
	EMPTY
};

/* ========================================================================== */
/**
 *	\brief	:	Contains the IDs of byte of password state.
 *	\see	:	APP_vFirstRunState.
 **/
enum
{
	password_existed	= 0xAA,
	password_not_exist	= 0xFF
};

/**************************** PREPROCESSOR MACROS *****************************/

/**
 *	\note 	:	Generally, macros are used to avoid using magic numbers in the
 * 				code. Be aware that macros are just a text replacement!
 **/

/* Macros of UART-related parameters */
#define UART_BAUD_RATE	9600
#define RX_FRAME_SIZE	5
#define TX_FRAME_SIZE	2

/* Macros of TWI-related parameters */
#define TWI_ADDRESS				10
#define TWI_BAUD_RATE			400000UL

/* Macros of EEPROM-related parameters */
#define EEPROM_PASS_STATE_ADDR	0x09
#define EEPROM_PASS_BYTE1_ADDR	0x0A

/* Macros to be used in byte separation and concatenation processes */
#define BYTES_PER_UINT32	4
#define BITS_PER_BYTE		8
#define INDEX_SHIFT			1

/* Macros to define the maximum numbers if invalid trials */
#define MAX_INVALID_TRIALS	3

/*************************** FUNCTIONS PROTOTYPE ******************************/

/**
 *	\brief	:	Iniitiate the application.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_init(void);

/**
 *	\brief	:	Check if EEPROM stores an existed password or not then determine the next state accordingly.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = FIRST_RUN.
 **/
void APP_vFirstRunState(void);

/**
 *	\brief	:	Recieve The new/chenged password from the HMI using the ready RX frame.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = NEW_PASSWORD.
 **/
void APP_vNewPasswordState(void);

/**
 *	\brief	:	Recieve The password from the HMI using the ready RX frame then check if equal to the g_password.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = CONFIRM_PASSWORD.
 **/
void APP_vConfirmNewPasswordState(void);

/**
 *	\brief	:	Save the password to EEPROM.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MATCHED_PASSWORD.
 **/
void APP_vMatchedPasswordState(void);

/**
 *	\brief	:	Handling the mismatch case of confirming any new password.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MISMACHTED_PASSWORD.
 **/
void APP_vMisMacthedPasswordState(void);

/**
 *	\brief	:	Get the right password from user (should be = password in EEPROM) to be able to give a command.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = ENTER_PASSWORD.
 **/
void APP_vEnterThePasswordState(void);

/**
 *	\brief	:	Get the desired command from user (+ for openning the door, or - to change the password).
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MAIN_MENU.
 **/
void APP_vMainMenuState(void);

/**
 *	\brief	:	Open the door CW for 15 seconds and 100% speed.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_OPEN.
 **/
void APP_vDoorOpenState(void);

/**
 *	\brief	:	Hold the door for 3 seconds.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_HOLD.
 **/
void APP_vDoorHoldState(void);

/**
 *	\brief	:	Open the door A-CW for 15 seconds and 100% speed
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_CLOSE.
 **/
void APP_vDoorCloseState(void);

/**
 *	\brief	:	Handles the wrong password entry.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = WRONG_PASSWORD.
 **/
void APP_vWrongPasswordState(void);

/**
 *	\brief	:	BUZZING for 1 minute! then try to re-enter the password
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = LOCK_SYSTEM.
 **/
void APP_vLockSystemState(void);

/**
 *	\brief	:	Concatenate the 4 bytes recieved from UART and return its value.
 *	\param	:	void.
 *	\return	:	uint32 password from EEPROM.
 **/
uint32 APP_u32RecievePasswordUART(void);

/**
 *	\brief	:	Concatenate the 4 bytes recieved from EEPROM and return its value.
 *	\param	:	void.
 *	\return	:	uint32 password from EEPROM.
 **/
uint32 APP_u32PasswordInEEPROM(void);

/**
 *	\brief	:	Separates the g_password then send it byte by byte to the EEPROM
 *				and update state in the EEPROM.
 *	\param	:	void.
 *	\return	:	uint32 password from EEPROM.
 **/
void APP_vSendPasswordToEEPROM(void);

/**
 *	\brief	:	The callback function of timer1 normal mode ISR, maintaining
 *				restarting with the same configuration every time TCNT1 resetted!
 *				When ISR occurs, the main task here is to decrement the externed
 *				global variable from timer1.c timer1_counter by one. Originally,
 *				this conter have an initial value equal to the needed time in
 *				seconds and decrements until the zero as an indicator to the
 *				desired time have been completed. \see timer1.c for more information.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vTimer1NormalCallback(void);

/**
 *	\brief	:	The callback function of timer1 CTC mode ISR. The only task here
 *				is to decrement the externed global variable from timer1.c
 *				timer1_counter by one. Originally, this conter have an initial
 *				value equal to the needed time in seconds and decrements until
 *				the zero as an indicator to the desired time have been completed
 *				and de-init the timer. \see timer1.c for more information.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vTimer1CTCCallback(void);

/**
 *	\brief	:	The callback function of UART_RXC ISR. The three tasks impelemented
 *				here are:
 *					1: Assign each received byte to its corresponding index in RX
 *					   frame.
 *					2: Decrement the g_bytes_recieved which initially valued with
 *					   the maximum number needed of bytes per frame. Then check if
 *					   reached the zero to:
 *					3: Raise up the RX flag.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vUartRxCallback(void);

/**
 *	\brief	:	The callback function of INT2 ISR. The two tasks here are to soft
 *				reset the HMI_ECU after reset the M24C16 EEPROM to its initial
 *				value. Its worth to mention that INT2 here is shared with the
 *				INT2 of HMI_ECU; thus, they will reset together.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vINT2softResetCallBack(void);

#endif /* INC_APPLICATION_H_ */
