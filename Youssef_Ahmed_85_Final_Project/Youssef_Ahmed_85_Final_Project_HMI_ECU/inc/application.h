/************************ PREPROCESSOR HEADER GUARD ***************************/
/*
 * 	A preprocessor directive used to prevent a header file from being included
 * 	multiple times in the same compilation unit, which could lead to issues like
 * 	redefinition errors.
 */
#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

/************************** HEADER FILES INCLUDES *****************************/

/* Used to import the externed g_current_hmi_state */
#include <main.h>

/* Used to use TIMER1 functions */
#include <timer1.h>

/* Used to use GPIO functions */
#include <gpio.h>

/* Used to use UART functions */
#include <uart.h>

/* Used to use INT2 functions */
#include <int2.h>

/* Used to use LCD functions */
#include <lcd.h>

/* Used to use KEYPAD functions */
#include <keypad.h>

/* Used to use standrad datatypes (boolean, uin8, ..) */
#include <std_types.h>

/* Used to generate a delay interval */
#include <util/delay.h>

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
 *	\brief	:	A modified version of system states to be used in some app functions.
 *	\see	:	systemStateType enum in main.h,
 *				APP_vCheckSuccessSyncTwoStates,
 *				APP_vCheckSuccessSyncThreeStates.
 **/
typedef enum
{
	_FIRST_RUN			,
	_NEW_PASSWORD		,
	_CONFIRM_PASSWORD	,
	_MATCHED_PASSWORD	,
	_MISMACHTED_PASSWORD,
	_ENTER_PASSWORD		,
	_MAIN_MENU			,
	_DOOR_OPEN			,
	_DOOR_HOLD			,
	_DOOR_CLOSE			,
	_WRONG_PASSWORD		,
	_LOCK_SYSTEM
} systemStateType_;

/* ========================================================================== */
/**
 *	\brief	:	Contains the IDs of messages will be displayed.
 *	\see	:	APP_displayMessage.
 **/
typedef enum
{
	MSG_FIRST_RUN	,
	MSG_PSWRD_INFO	,
	MSG_NEW_PSWRD	,
	MSG_CFRM_PSWRD	,
	MSG_MTCH_PSWRD	,
	MSG_MSMT_PSWRD	,
	MSG_ENTR_PSWRD	,
	MSG_MAIN_MENU	,
	MSG_DOOR_OPEN	,
	MSG_DOOR_HOLD	,
	MSG_DOOR_CLOS	,
	MSG_WRNG_PSWRD	,
	MSG_LOCK_SYS
} messagesType;

/* ========================================================================== */
/**
 *	\brief	:	Contains the indices of the password entered by the user.
 *	\see	:	APP_vGetPasswordFromUser.
 **/
typedef enum
{
	DIGIT_0,
	DIGIT_1,
	DIGIT_2,
	DIGIT_3,
	DIGIT_4,
	DIGIT_5
} passwordDigitsType;

/* ========================================================================== */
/**
 *	\brief	:	Contains the IDs valid KEYPAD button as password numbers.
 *	\see	:	APP_vGetPasswordFromUser.
 **/
typedef enum
{
	KP_ZERO,
	KP_ONE,
	KP_TWO,
	KP_THREE,
	KP_FOUR,
	KP_FIVE,
	KP_SIX,
	KP_SEVEN,
	KP_EIGHT,
	KP_NINE
} KEYPAD_numbersType;

/**************************** PREPROCESSOR MACROS *****************************/

/**
 *	\note 	:	Generally, macros are used to avoid using magic numbers in the
 * 				code. Be aware that macros are just a text replacement!
 **/

/* Macros of UART-related parameters */
#define UART_BAUD_RATE	9600
#define RX_FRAME_SIZE	2
#define TX_FRAME_SIZE	5

/* Macros to set the fixed location of password's 1st-digit on LCD */
#define PASSWORD_ROW	3
#define PASSWORD_COL	10

/* Macros defining multipliers for each digit's position in the password */
#define MULTIPLIER_1		1
#define MULTIPLIER_10		10
#define MULTIPLIER_100		100
#define MULTIPLIER_1000		1000
#define MULTIPLIER_10000	10000

/* Macros to be used in byte separation and concatenation processes */
#define BYTES_PER_UINT32	4
#define BITS_PER_BYTE		8

/*
 * A very small delay by util/delay.h in milliseconds to be used while getting
 * the password from the user!
 */
#define DELAY_FIFTY_MS	_delay_ms(50)

/*************************** FUNCTIONS PROTOTYPE ******************************/

/**
 *	\brief	:	Iniitiate the application.
 *	\param	:	Void.
 *	\return	:	Void.
 **/
void APP_init(void);

/**
 *	\brief	:	Display the welcome message if CONTROL_ECU's CTRL byte was matched.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = FIRST_RUN.
 **/
void APP_vFirstRunState(void);

/**
 *	\brief	:	Display messages in case of set a new password,
 *				whether in 1st time or in change password state.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = NEW_PASSWORD.
 **/
void APP_vNewPasswordState(void);

/**
 *	\brief	:	Display messages to confirm the new password.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = CONFIRM_PASSWORD.
 **/
void APP_vConfirmNewPasswordState(void);

/**
 *	\brief	:	Display messages to confirm that your passwords matched.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MATCHED_PASSWORD.
 **/
void APP_vMatchedPasswordState(void);

/**
 *	\brief	:	Display messages to inform the user that the password and its
 *				confirmation are not matched. If mismatched 3 times, redo the
 *				enter new password state. If the passwords are matched, we will
 *				not navigate to this state.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MISMACHTED_PASSWORD.
 **/
void APP_vMisMacthedPasswordState(void);

/**
 *	\brief	:	Display messages to ask for the password before navigate to the
 *				main menu. This state will be visited if there is already a saved
 *				password only. Furthermore, this state will be visited after lock
 *				system state be completed. Morevoer, we will visit this state
 *				after the door closed in case if open/hold/close door states selected.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = ENTER_PASSWORD.
 **/
void APP_vEnterThePasswordState(void);

/**
 *	\brief	:	Display messages to ask for the desired command, whether to open
 *				the door or to change the current password and set a new password.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MAIN_MENU.
 **/
void APP_vMainMenuState(void);

/**
 *	\brief	:	Display message that the door is openning now. We will navigate
 *				here only if the user choose to open the door.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_OPEN.
 **/
void APP_vDoorOpenState(void);
/**
 *	\brief	:	Display message that the door is holding now. We will navigate here
 *				only after door been completely open
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_HOLD.
 **/
void APP_vDoorHoldState(void);

/**
 *	\brief	:	Display message that the door is holding now. We will navigate here
 *				only after door been completely open
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_CLOSE.
 **/
void APP_vDoorCloseState(void);

/**
 *	\brief	:	Display message that the enterd password was wrong, and it will
 *				be visited only in case of less than maximum number of invalid
 *				trials satisfied. Then re-set the state to enter password state.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = WRONG_PASSWORD.
 **/
void APP_vWrongPasswordState(void);

/**
 *	\brief	:	Display message that the system is locked for 1 minute. We will
 *				navigate here only if reached the maximum number of password's
 *				invalid trials. After lock the system, return to enter password
 *				state.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = LOCK_SYSTEM.
 **/
void APP_vLockSystemState(void);

/**
 *	\brief	:	Polling until the RX flag turns TRUE, then check if CTRL byte
 *				received from the CONTROL is success. if so, navigate to the state
 *				that stated if the 1st parameter. If an error byte was received,
 *				redo the current state, which is given as a 2nd parameter to this
 *				function. Its worth to mention that this function used in the end
 *				of each state function to ensure a safe sync with the CONTROL_ECU.
 *	\param 1:	state will be navigated if CTRL_SUCCESS.
 *	\param 2:	state will be repeated if CTRL_ERROR.
 *	\return	:	void.
 **/
void APP_vCheckSuccessSyncOneState(systemStateType_ if_success, systemStateType_ if_fail);

/**
 *	\brief	:	Same to APP_vCheckSuccessSyncOneState, but it takes two more
 *				parameters, which are the received CTRL byte1 (after byte0 success or
 *				error) and the state will be navigated if the CTRL byte1 is not
 *				referring to the other state.
 *	\param 1:	The expected CTRL byte1, which will determine which parameter will win!
 *	\param 2:	state will be navigated if byte0 is CTRL_SUCCESS and byte1 refers to it.
 *	\param 3:	state will be navigated if byte0 is CTRL_SUCCESS and byte1 did no refer
 *				to the other state.
 *	\param 4:	state will be repeated if CTRL_ERROR.
 *	\return	:	void.
 **/
void APP_vCheckSuccessSyncTwoStates(	UART_ControlByteType	CTRL_if_success1	,\
										systemStateType_		if_success1		  	,\
										systemStateType_		if_success2			,\
										systemStateType_		if_fail				);

/**
 *	\brief	:	Same to APP_vCheckSuccessSyncTwoStates, but it takes two more
 *				parameters, which are the 2nd expected state to be navigated, and
 *				the state will be navigated if the CTRL byte1 is not referring to
 *				any of the expected states.
 *	\param 1:	The expected CTRL byte1, which will determine which parameter will win!
 *	\param 2:	state will be navigated if byte0 is CTRL_SUCCESS and byte1 refers to it.
 *	\param 3:	state will be navigated if byte0 is CTRL_SUCCESS and byte1 refers to it.
 *	\param 4:	state will be navigated if byte0 is CTRL_SUCCESS and byte1 did no refer
 *				to any of the other states.
 *	\param 5:	state will be repeated if CTRL_ERROR.
 *	\return	:	void.
 **/
void APP_vCheckSuccessSyncThreeStates(	UART_ControlByteType	CTRL_if_success1	,\
										UART_ControlByteType	CTRL_if_success2	,\
										systemStateType_		if_success1		  	,\
										systemStateType_		if_success2			,\
										systemStateType_		if_success3			,\
										systemStateType_		if_fail				);

/**
 *	\brief	:	Updates the TX frame with one CTRL byte0, which indicates the current
 *				HMI state, and EMPTY bytes from byte1 to byte5, this because a fixed
 *				size of TX frame should be achieved. If the desired frame to be
 *				updated is the main menu, then byte1 will not be EMPTY, alternatively,
 *				we will execute the function of handling user command. Generally,
 *				This function maintain sending 5 bytes even if not totally needed,
 *				but only to keep success sync with CONTROL_ECU which will be polling
 *				until get a full 5-bytes- frame from the HMI_ECU.
 *	\param 1:	The current state of HMI_ECU.
 *	\return	:	void.
 **/
void APP_vSendCurrentHMIStateUART(frameType byte_0);

/**
 *	\brief	:	Calls the function of getting user password, then separate it from
 *				four byte (sizeof(uint32) = 4 bytes) into four different bytes.
 *				After updating the TX frame byte-by-byte, use UART send TX frame
 *				function to transmit this updated frame.
 *	\param 1:	CTRL byte that indicates the current state.
 *	\return	:	void.
 **/
void APP_vGetPasswordFromUserThenSendUART(UART_ControlByteType control_byte);

/**
 *	\brief	:	Calls the function of getting user password, then update the TX
 *				frame and send it, including the desired user command.
 *	\param :	void.
 *	\return	:	void.
 **/
void APP_vGetCommandFromUserThenSendUART(void);

/**
 *	\brief	:	Displays a predefined messages according to the given message ID.
 *				This function is the beginning of each system state function (except
 *				the APP_vFirstRunState function) to make sure that the user will not
 *				keep awaiting the CONTROL_ECU decisions with an empty screen!
 *	\param 1:	The desired message ID to display on the LCD.
 *	\return	:	void.
 **/
void APP_displayMessage(messagesType message_ID);

/**
 *	\brief	:	Polling until get an accepted password from the user, If any of
 *				unaccepted inputs occur, it will be reiterated. Moreover, this
 *				function will display the selected number for 50 milliseconds to
 *				make the user aware with the number he entered then hide it with
 *				an asterisk (*) character. Furthermore, This function directly
 *				updates the value of g_password digit-by-digit. The mechanism
 *				of updating the password is by check the current digit of the
 *				password and multiply it to the corresponding power of ten that
 *				have the number of zeros suitable to place this number in its
 *				correct position, \see the example below.
 *				Since it is coverable by the uint32 range, we can handle it as
 *				4 bytes variable, instead (for exmaple) assign each digit to an
 *				index of a uin8 array, which will lead to consume one more byte.
 *	\example:	Password update example: password:	75962:
 *					Digit 1: 7 * 10000			=	70000
 *					Digit 2: 5 * 1000	+ 70000	=	75000
 *					Digit 3: 9 * 100	+ 75000	=	75900
 *					Digit 4: 6 * 10		+ 75900	=	75960
 *					Digit 5: 2 * 1		+ 75960	=	75962
 *	\note	:	The constraints of password are:
 *					1. Do not start with ZERO!
 *					2. Do not enter any special character.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vGetPasswordFromUser(void);

/**
 *	\brief	:	Get only accepted command form user (+ to open the door, and - to
 *				change the password) and return this command to be used later.
 *	\param 	:	void.
 *	\return	:	The entered command by user.
 **/
uint8 APP_u8GetCommandFromUser(void);

/**
 *	\brief		:	Get the TX frame elemtns and assign them to the desired array
 *					location, then send them all.
 *	\param 1-5	:	The elements of TX frame.
 *	\return		:	void.
 **/
void APP_vUpdateTXFrameThenSendUART(frameType byte_0, frameType byte_1, frameType byte_2, frameType byte_3, frameType byte_4);

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
 *					1: Assign each received byte to its corresponding inex in RX frame.
 *					2: Decrement the g_bytes_recieved which initially valued with
 *					   the maximum number needed of bytes per frame. Then check if
 *					   reached the zero to:
 *					3: Raise up the RX flag.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vUartRxCallback(void);

/**
 *	\brief	:	The callback function of INT2 ISR. The only task here is to soft
 *				reset the HMI_ECU. Its worth to mention that INT2 here is shared
 *				with the INT2 of CONTROL_ECU; thus, they will reset together.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vINT2softResetCallBack(void);

#endif /* INC_APPLICATION_H_ */
