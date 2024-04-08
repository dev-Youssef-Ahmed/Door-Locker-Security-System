/*******************************************************************************
 *				Final Project	:	Door Locker Security System				   *
 *				File of			:	CONTROL ECU	- application functions		   *
 *				Facebook Name	:	Youssef Ahmed							   *
 *				Diploma Number	:	85				 						   *
 ******************************************************************************/

/************************************************************
 * \note: set your timer1 mode and times (in seconds) here!	*
 * \note: JUST COPY YOUR DESIRED VALUE FROM THE				*
 * 		  COMMENT UPSIDE EACH FOLLOWING #define 			*
 ***********************************************************/

#warning: DO NOT FORGET TO SYNC THE TIME OF HMI_ECU/src/application.c

/* Choose your mode, but should be one of: NORMAL or CTC */
#define TIMER1_MODE_OF_OPERATION			CTC
/* Required: FIFTEEN_SECONDS,	or for Test: THREE_SECONDS */
#define TIMER1_DOOR_OPEN_OR_CLOSE			THREE_SECONDS
/* Required: THREE_SECONDS,		or for Test: ONE_SECOND */
#define TIMER1_DOOR_HOLD_AND_LCD_MESSAGES	ONE_SECOND
/* Required: SIXTY_SECONDS,		or for Test: TEN_SECONDS */
#define TIMER1_BUZZER_ON_AND_SYSTEM_LOCK	TEN_SECONDS
/************************************************************
 *\note: no need to change anymore values later in the code!*
 ***********************************************************/

/************************** HEADER FILES INCLUDES *****************************/

/* Include the necessary header file */
#include <application.h>

/**************************** GLOBAL VARIABLES ********************************/

/* Holds the number of received bytes, decrements from 4 to 0 */
uint8	g_bytes_recieved;

/* Holds TRUE if 5 bytes are received, then changes to FALSE */
boolean	g_RX_ready_flag;

/*
 * Holds received 5 bytes:
 *
 * In case of receiving the uint32 g_password:
 * 		[Byte0: CTRL] [Byte1: Password Byte 0] [Byte2: Password Byte 1]
 * 		[Byte3: Password Byte 2] [Byte4: Password Byte 3]
 *
 * In case of receiving user's command:
 * 		[Byte0: CTRL] [Byte1: Command] [Bytes 2-4: EMPTY]
 *
 * In case of receiving the current state:
 * 		[Byte0: CTRL] [Bytes 1-4: EMPTY]
 */
uint8	g_RX_frame[RX_FRAME_SIZE];

/*
 * Holds the received  2 bytes:
 *		[Byte0: Success or Fail] [Byte1: CTRL]
 */
uint8	g_TX_frame[TX_FRAME_SIZE];

/* Holds the password that will be transmitted to CONTROL_ECU */
uint32	g_password;

/* Holds the current password state if existed or not */
uint8	g_password_state;

/* Holds the current times to fault password enter */
uint8	g_invalid_enter_password_trials;

/* Holds the current times to mismatched new-passwrod confirmation */
uint8	g_invalid_mismatche_trials;

/**************************** APP INITIALIZATION ******************************/

/**
 *	\brief	:	Iniitiate the application.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_init(void)
{
	/* Enter the first run system state, which will determine the following steps */
	g_current_control_state = FIRST_RUN;

	/* ====================================================================== */
	/*							 Timer1 configurations:						  */
	/* ====================================================================== */

	/* Holds the desired Timer1 mode */
	g_Timer1_mode_of_operation = TIMER1_MODE_OF_OPERATION;

	/*
	 * Configure the callback functions for Timer1 ISRs, this
	 * enable flexibility in event handling and customization.
	 */
	Timer1_CallbackType	TimerCallback		= {APP_vTimer1NormalCallback, APP_vTimer1CTCCallback};
	Timer1_setCallBack(&TimerCallback);

	/* ====================================================================== */
	/*						 Interrupt 2 configurations:					  */
	/* ====================================================================== */

	/* Initiate the parameters of INT2, which is responsible for RESET process */
	INT2_init();
	/* Select the callback function that will be called in INT2 ISR occurrences */
	INT2_setCallBack(APP_vINT2softResetCallBack);

	/* ====================================================================== */
	/* 						UART & TWI configurations:						  */
	/* ====================================================================== */

	/* Initial vlues: */
	g_RX_ready_flag		= FALSE;
	g_bytes_recieved	= RX_FRAME_SIZE;

	/*
	 * Configuration struct members (in order):
	 * {character size, parity, stop bits, baud rate}
	 */
	UART_ConfigType		UART_configurations	= {DATA_8_BIT, EVEN_PARITY, STOP_BIT_1, UART_BAUD_RATE};
	UART_Init(&UART_configurations);

	/* Select the callback function that will be called in UART_RXC ISR occurrences */
	UARTRX_setCallBack(APP_vUartRxCallback);

	TWI_ConfigType		TWI_configurations	= {TWI_ADDRESS, TWI_BAUD_RATE};
	TWI_init(&TWI_configurations);

	/* ====================================================================== */
	/* 						Components initializations:						  */
	/* ====================================================================== */

	/* GPIO is a static configurable in buzzer.h */
	Buzzer_init();

	/* GPIO is a static configurable in motor.h */
	DcMotor_Init();

	/* ====================================================================== */
	/*		General initial values for global invalid trials variables		  */
	/* ====================================================================== */

	/* The initial value of the decremental password conformation mismatching trials */
	g_invalid_mismatche_trials = MAX_INVALID_TRIALS;

	/* The initial value of the decremental password invalid entering trials */
	g_invalid_enter_password_trials = MAX_INVALID_TRIALS;

	/* ====================================================================== */
	/*							Enable global interrupt						  */
	/* ====================================================================== */
	sei();
}

/************************ SYSTEM STATES FUNCTIONS *****************************/

/* ========================================================================== */
/**
 *	\brief	:	Check if EEPROM stores an existed password or not then determine the next state accordingly.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = FIRST_RUN.
 **/
void APP_vFirstRunState(void)
{
	/* Read the password state address in the EEPROM then update the global flag */
	EEPROM_readByte(EEPROM_PASS_STATE_ADDR, &g_password_state);

	/* Check the final decision of password state */
	if(g_password_state == password_existed || g_password_state == password_not_exist)
	{
		/* If password not previously entered, lets start getting a new password */
		if (g_password_state == password_not_exist)
		{
			/* CONTROL_ECU, end the current state and go to this next state! */
			g_current_control_state = NEW_PASSWORD;

			/* Send a control byte indicates successful, making HMI ready to update its next system state */
			UART_sendByte(CTRL_SUCCESS);
			/* Send a control byte that indicates the HMI for the new current state */
			UART_sendByte(CTRL_GET_NEW_PASSWORD);
		}
		/* If password already existing, lets enter it */
		else
		{
			/* CONTROL_ECU, end the current state and go to this next state! */
			g_current_control_state = ENTER_PASSWORD;

			/* Send a control byte indicates successful, making HMI ready to update its next system state */
			UART_sendByte(CTRL_SUCCESS);
			/* Send a control byte that indicates the HMI for the new current state */
			UART_sendByte(CTRL_ENTER_PASSWORD);
		}
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = FIRST_RUN;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state, only first run have EMPTY error byte */
		UART_sendByte(CTRL_EMPTY);
	}
}

/* ========================================================================== */
/**
 *	\brief	:	Recieve The new/chenged password from the HMI using the ready RX frame.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = NEW_PASSWORD.
 **/
void APP_vNewPasswordState(void)
{
	/* Reset the invalid trials if navigated here! */
	g_invalid_enter_password_trials = MAX_INVALID_TRIALS;

	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);

	/* Maintain sync. by check the income control byte from the HMI */
	if (g_RX_frame[BYTE_0_CONTROL] == CTRL_GET_NEW_PASSWORD)
	{
		/* Concatenate the 4 bytes recieved from HMI through the UART to update the g_password */
		g_password = APP_u32RecievePasswordUART();

		/* CONTROL_ECU, end the current state and go to this next state! */
		g_current_control_state = CONFIRM_PASSWORD;

		/* Send a control byte indicates successful, making HMI ready to update its next system state */
		UART_sendByte(CTRL_SUCCESS);
		UART_sendByte(CTRL_EMPTY);
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = NEW_PASSWORD;

		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_GET_NEW_PASSWORD);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	Recieve The password from the HMI using the ready RX frame then check if equal to the g_password.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = CONFIRM_PASSWORD.
 **/
void APP_vConfirmNewPasswordState(void)
{
	/* Holds the g_password temporarily as a backup */
	uint32 temp_password = g_password;

	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);

	/* Maintain sync. by check the income control byte from the HMI */
	if (g_RX_frame[BYTE_0_CONTROL] == CTRL_CONFIRM_NEW_PASSWORD)
	{
		/* Concatenate the 4 bytes recieved from HMI through the UART to update the g_password */
		g_password = APP_u32RecievePasswordUART();

		/* Check if the last recieved password is matched to the backup temp_password or not */
		if(g_password == temp_password)
		{
			/* CONTROL_ECU, end the current state and go to this next state! */
			g_current_control_state = MATCHED_PASSWORD;
			/* Send a control byte indicates successful, making HMI ready to update its next system state */
			UART_sendByte(CTRL_SUCCESS);
			/* Send a control byte that indicates the HMI for the new current state */
			UART_sendByte(CTRL_PASSWORD_MATCHED);
		}
		else
		{
			/* Restore the original g_password to be re-usable in the next state */
			g_password = temp_password;
			/* CONTROL_ECU, end the current state and go to this next state! */
			g_current_control_state = MISMACHTED_PASSWORD;
			/* Send a control byte indicates successful, making HMI ready to update its next system state */
			UART_sendByte(CTRL_SUCCESS);
			/* Send a control byte that indicates the HMI for the new current state */
			UART_sendByte(CTRL_PASSWORD_MISMATCHED);
		}
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = CONFIRM_PASSWORD;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_CONFIRM_NEW_PASSWORD);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	Save the password to EEPROM.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MATCHED_PASSWORD.
 **/
void APP_vMatchedPasswordState(void)
{
	/* Holds the password saved in the EEPROM temporarily to check it */
	uint32	temp_password;
	/* Holds the password state saved in the EEPROM temporarily to check it */
	uint8	temp_state;
	/* Holds the invalid trials to write the password into the EEPROM */
	uint8	invalid_eeprom_trials = ONE;
	/* Resets any previous invalid confirming password */
	g_invalid_mismatche_trials = ZERO;

	/* Do the follwing instruction then check if reached the maximum number of invalid trials */
	do
	{
		/* Send the g_password to EEPROM and update the password state inside too */
		APP_vSendPasswordToEEPROM();

		/* update the password state of CONTROL_ECU */
		g_password_state = password_existed;

		/* === FOR SAFETY CHECHK === */
		/* In the following two steps will update the local vriables with the EEPROM values */
		temp_password = APP_u32PasswordInEEPROM();
		EEPROM_readByte(EEPROM_PASS_STATE_ADDR, &temp_state);

		/* Check if saved to EEPROM successfully */
		if((temp_password == g_password) && (temp_state == (uint8)g_password_state))
		{
			/* CONTROL_ECU, end the current state and go to this next state! */
			g_current_control_state = ENTER_PASSWORD;
			/* Send a control byte indicates successful, making HMI ready to update its next system state */
			UART_sendByte(CTRL_SUCCESS);
			UART_sendByte(CTRL_EMPTY);
			break;
		}
		else
		{
			/*
			 * Increment the invalid EEPROM trials, then satisfy the do while loop
			 * Unless it reaches the maximum number of invalid trials the pass to
			 * the following if condition.
			 */
			invalid_eeprom_trials++;

			/* Navigate here only if maximum occured */
			if(invalid_eeprom_trials == MAX_INVALID_TRIALS)
			{
				/* Give a last chance! */
				APP_vSendPasswordToEEPROM();
				g_password_state = password_existed;

				temp_password = APP_u32PasswordInEEPROM();
				EEPROM_readByte(EEPROM_PASS_STATE_ADDR, &temp_state);

				/* Check if last chance successed! */
				if((temp_password == g_password) && (temp_state == (uint8)g_password_state))
				{
					/* CONTROL_ECU, end the current state and go to this next state! */
					g_current_control_state = ENTER_PASSWORD;
					/* Send a control byte indicates successful, making HMI ready to update its next system state */
					UART_sendByte(CTRL_SUCCESS);
					UART_sendByte(CTRL_EMPTY);
					break;
				}
				/* Trials suspended, retry to save form the beginning */
				else
				{
					/* CONTROL_ECU, redo this state! */
					g_current_control_state = MATCHED_PASSWORD;
					/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
					UART_sendByte(CTRL_ERROR);
					/* Send a control byte that refers to the error state */
					UART_sendByte(CTRL_PASSWORD_MATCHED);
				}
			}
		}
	} while(invalid_eeprom_trials < MAX_INVALID_TRIALS);
}

/* ========================================================================== */
/**
 *	\brief	:	Handling the mismatch case of confirming any new password.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MISMACHTED_PASSWORD.
 **/
void APP_vMisMacthedPasswordState(void)
{
	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);

	/* Maintain sync. by check the income control byte from the HMI */
	if(g_RX_frame[BYTE_0_CONTROL] == CTRL_PASSWORD_MISMATCHED)
	{

		if(--g_invalid_mismatche_trials > ZERO)
		{
			/* CONTROL_ECU, end the current state and go to this next state! */
			g_current_control_state = CONFIRM_PASSWORD;
			/* Send a control byte indicates successful, making HMI ready to update its next system state */
			UART_sendByte(CTRL_SUCCESS);
			/* Send a control byte that indicates the HMI for the new current state */
			UART_sendByte(CTRL_CONFIRM_NEW_PASSWORD);
		}
		else
		{
			/* CONTROL_ECU, end the current state and go to this next state! */
			g_current_control_state = NEW_PASSWORD;
			/* Send a control byte indicates successful, making HMI ready to update its next system state */
			UART_sendByte(CTRL_SUCCESS);
			/* Send a control byte that indicates the HMI for the new current state */
			UART_sendByte(CTRL_GET_NEW_PASSWORD);
		}
	}
	else
	{
		/* CONTROL_ECU, rdo this state! */
		g_current_control_state = MISMACHTED_PASSWORD;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_PASSWORD_MISMATCHED);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	Get the right password from user (should be = password in EEPROM) to be able to give a command.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = ENTER_PASSWORD.
 **/
void APP_vEnterThePasswordState(void)
{
	/* A temp password that holds the password extracted to the EEPROM */
	uint32 temp_password = ZERO;
	g_password = ZERO;

	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);

	/* Maintain sync. by check the income control byte from the HMI */
	if(g_RX_frame[BYTE_0_CONTROL] == CTRL_ENTER_PASSWORD)
	{
		/* Reference password */
		g_password	 = APP_u32PasswordInEEPROM();
		/* User's password */
		temp_password= APP_u32RecievePasswordUART();

		/* Check if they're matching! */
		if(g_password == temp_password)
		{
			/* Reset the invalid trials if navigated here! */
			g_invalid_enter_password_trials = MAX_INVALID_TRIALS;
			/* CONTROL_ECU, end the current state and go to this next state! */
			g_current_control_state = MAIN_MENU;
			/* Send a control byte indicates successful, making HMI ready to update its next system state */
			UART_sendByte(CTRL_SUCCESS);
			/* Send a control byte that indicates the HMI for the new current state */
			UART_sendByte(CTRL_MAIN_MENU);
		}
		/* Entered a wrong password! take care, you maybe a theif! */
		else
		{
			if(--g_invalid_enter_password_trials > ZERO)
			{
				/* CONTROL_ECU, end the current state and go to this next state! */
				g_current_control_state = WRONG_PASSWORD;
				/* Send a control byte indicates successful, making HMI ready to update its next system state */
				UART_sendByte(CTRL_SUCCESS);
				/* Send a control byte that indicates the HMI for the new current state */
				UART_sendByte(CTRL_WRONG_PASSWORD);
			}
			else
			{
				/* CONTROL_ECU, end the current state and go to this next state! */
				g_current_control_state = LOCK_SYSTEM;
				/* Send a control byte indicates successful, making HMI ready to update its next system state */
				UART_sendByte(CTRL_SUCCESS);
				/* Send a control byte that indicates the HMI for the new current state */
				UART_sendByte(CTRL_LOCK_SYSTEM);
			}
		}
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = ENTER_PASSWORD;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_ENTER_PASSWORD);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	Get the desired command from user (+ for openning the door, or - to change the password).
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MAIN_MENU.
 **/
void APP_vMainMenuState(void)
{
	/* Holds the command that user asked for */
	userCommandsType user_command;

	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);

	/* Maintain sync. by check the income control byte from the HMI */
	if(g_RX_frame[BYTE_0_CONTROL] == CTRL_MAIN_MENU)
	{
		/* Get the 2nd byte of the RX frame and enusre to cast it as a userCommandType */
		user_command = (userCommandsType)(g_RX_frame[BYTE_1_PASS0_OR_CMD]);

		/* Check if + for openning the door, or - to change the password */
		switch(user_command)
		{
			case CMD_OPN_DOOR	:
			{
				/* CONTROL_ECU, end the current state and go to this next state! */
				g_current_control_state = DOOR_OPEN;
				/* Send a control byte indicates successful, making HMI ready to update its next system state */
				UART_sendByte(CTRL_SUCCESS);
				/* Send a control byte that indicates the HMI for the new current state */
				UART_sendByte(CTRL_DOOR_OPEN);
				break;
			}
			case CMD_CHNG_PSWRD	:
			{
				/* CONTROL_ECU, end the current state and go to this next state! */
				g_current_control_state = NEW_PASSWORD;
				/* Send a control byte indicates successful, making HMI ready to update its next system state */
				UART_sendByte(CTRL_SUCCESS);
				/* Send a control byte that indicates the HMI for the new current state */
				UART_sendByte(CTRL_GET_NEW_PASSWORD);
				break;
			}
		}
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = MAIN_MENU;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_MAIN_MENU);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	Open the door CW for 15 seconds and 100% speed.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_OPEN.
 **/
void APP_vDoorOpenState(void)
{
	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);

	/* Maintain sync. by check the income control byte from the HMI */
	if(g_RX_frame[BYTE_0_CONTROL] == CTRL_DOOR_OPEN)
	{
		/* Start motor openning process */
		DcMotor_Rotate(MOTOR_DIRECTION_CW, MOTOR_SPEED_100);
		/* Start a 15 seconds timer */
		TIMER1_vStartTimer(TIMER1_DOOR_OPEN_OR_CLOSE);

		/* CONTROL_ECU, end the current state and go to this next state! */
		g_current_control_state = DOOR_HOLD;

		/* Send a control byte indicates successful, making HMI ready to update its next system state */
		UART_sendByte(CTRL_SUCCESS);
		UART_sendByte(CTRL_EMPTY);
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = DOOR_OPEN;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_DOOR_OPEN);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	Hold the door for 3 seconds.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_HOLD.
 **/
void APP_vDoorHoldState(void)
{
	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);

	/* Maintain sync. by check the income control byte from the HMI */
	if(g_RX_frame[BYTE_0_CONTROL] == CTRL_DOOR_HOLD)
	{
		/* Start motor holding process */
		DcMotor_Rotate(MOTOR_DIRECTION_OFF, MOTOR_SPEED_OFF);

		/* Start a 3 seconds timer */
		TIMER1_vStartTimer(TIMER1_DOOR_HOLD_AND_LCD_MESSAGES);

		/* CONTROL_ECU, end the current state and go to this next state! */
		g_current_control_state = DOOR_CLOSE;

		/* Send a control byte indicates successful, making HMI ready to update its next system state */
		UART_sendByte(CTRL_SUCCESS);
		UART_sendByte(CTRL_EMPTY);
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = DOOR_HOLD;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_DOOR_HOLD);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	Open the door A-CW for 15 seconds and 100% speed
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_CLOSE.
 **/
void APP_vDoorCloseState(void)
{
	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);

	/* Maintain sync. by check the income control byte from the HMI */
	if(g_RX_frame[BYTE_0_CONTROL] == CTRL_DOOR_CLOSE)
	{
		/* Start motor holding process */
		DcMotor_Rotate(MOTOR_DIRECTION_ACW, MOTOR_SPEED_100);

		/* Start a 15 seconds timer */
		TIMER1_vStartTimer(TIMER1_DOOR_OPEN_OR_CLOSE);

		/* Stop the motor */
		DcMotor_Rotate(MOTOR_DIRECTION_OFF, MOTOR_SPEED_OFF);

		/* CONTROL_ECU, end the current state and go to this next state! */
		g_current_control_state = ENTER_PASSWORD;

		UART_sendByte(CTRL_SUCCESS);
		UART_sendByte(CTRL_EMPTY);
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = DOOR_CLOSE;
		/* Send a control byte indicates successful, making HMI ready to update its next system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_DOOR_CLOSE);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	Handles the wrong password entry.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = WRONG_PASSWORD.
 **/
void APP_vWrongPasswordState(void)
{
	while(g_RX_ready_flag == FALSE);
	if(g_RX_frame[BYTE_0_CONTROL] == CTRL_WRONG_PASSWORD)
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = ENTER_PASSWORD;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_SUCCESS);
		UART_sendByte(CTRL_EMPTY);
	}
	else
	{
		/* CONTROL_ECU, redo this state! */
		g_current_control_state = WRONG_PASSWORD;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_WRONG_PASSWORD);
	}

	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
/**
 *	\brief	:	BUZZING for 1 minute! then try to re-enter the password
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = LOCK_SYSTEM.
 **/
void APP_vLockSystemState(void)
{
	/* Polling until recieving a RX frame */
	while(g_RX_ready_flag == FALSE);
	/* Maintain sync. by check the income control byte from the HMI */
	if(g_RX_frame[BYTE_0_CONTROL] == CTRL_LOCK_SYSTEM)
	{
		/* Turn on the buzzer, your trails are suspected! */
		Buzzer_on();

		/* Start a 60 seconds timer */
		TIMER1_vStartTimer(TIMER1_BUZZER_ON_AND_SYSTEM_LOCK);

		/* Turn off the buzzer if it was active! */
		Buzzer_off();

		/* CONTROL_ECU, end the current state and go to this next state! */
		g_current_control_state = ENTER_PASSWORD;

		/* Send a control byte indicates successful, making HMI ready to update its next system state */
		UART_sendByte(CTRL_SUCCESS);
		UART_sendByte(CTRL_EMPTY);
	}
	else
	{
		/* CONTROL_ECU, we failed! redo the current state! */
		g_current_control_state = LOCK_SYSTEM;
		/* Send a control byte indicates error occured, making HMI ready to restaet its current system state */
		UART_sendByte(CTRL_ERROR);
		/* Send a control byte that refers to the error state */
		UART_sendByte(CTRL_LOCK_SYSTEM);
	}

	/* Clear the RX flag to be re-usable */
	g_RX_ready_flag = FALSE;
}

/****** APP INDIRECT-SUPPORT FUNCTIONS: Handling UART RX Frame Functions ******/

/* ========================================================================== */
/**
 *	\brief	:	Concatenate the 4 bytes recieved from UART and return its value.
 *	\param	:	void.
 *	\return	:	uint32 password from EEPROM.
 **/
uint32 APP_u32RecievePasswordUART(void)
{
	frameType	byte_index;
	uint8		byte;
	uint32		password = ZERO;
	/* Send the uint32 password byte by byte (32/8 = 4 iterations) */
	for(byte_index = BYTE_1_PASS0_OR_CMD ; byte_index <= BYTE_4_PASS3 ; byte_index++)
	{
		byte = g_RX_frame[BYTE_1_PASS0_OR_CMD + (byte_index - INDEX_SHIFT)];
		password |= ((uint32)(byte)) << ((BYTES_PER_UINT32 - byte_index) * BITS_PER_BYTE);
	}
	return password;
}

/******** APP INDIRECT-SUPPORT FUNCTIONS: EEPROM Read/Write Functions *********/

/* ========================================================================== */
/**
 *	\brief	:	Concatenate the 4 bytes recieved from EEPROM and return its value.
 *	\param	:	void.
 *	\return	:	uint32 password from EEPROM.
 **/
uint32 APP_u32PasswordInEEPROM(void)
{
	frameType	byte_index;
	uint8		byte;
	uint32		password = ZERO;
	/* Send the uint32 password byte by byte (32/8 = 4 iterations) */
	for(byte_index = BYTE_1_PASS0_OR_CMD ; byte_index <= BYTE_4_PASS3 ; byte_index++)
	{
		EEPROM_readByte((EEPROM_PASS_BYTE1_ADDR + (byte_index - INDEX_SHIFT)), &byte);
		password |= ((uint32)(byte)) << ((BYTES_PER_UINT32 - byte_index) * BITS_PER_BYTE);
	}

	return password;
}

/* ========================================================================== */
/**
 *	\brief	:	Separates the g_password then send it byte by byte to the EEPROM
 *				and update state in the EEPROM.
 *	\param	:	void.
 *	\return	:	uint32 password from EEPROM.
 **/
void APP_vSendPasswordToEEPROM(void)
{
	uint8 	byte_index;
	uint8	byte;
	uint8	password_state_in_EEPROM = password_existed;
	for(byte_index = BYTE_1_PASS0_OR_CMD ; byte_index <= BYTE_4_PASS3 ; byte_index++)
	{
		byte = (uint8)(g_password >> ((BYTES_PER_UINT32 - byte_index) * BITS_PER_BYTE));
		EEPROM_writeByte((EEPROM_PASS_BYTE1_ADDR + (byte_index - INDEX_SHIFT)), byte);
	}
	EEPROM_writeByte(EEPROM_PASS_STATE_ADDR, password_state_in_EEPROM);
}

/********** APP INDIRECT-SUPPORT FUNCTIONS: ISRs Callback Functions ***********/

/* ========================================================================== */
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
void APP_vTimer1NormalCallback(void)
{
	/*
	 * Every time ISR occurs, we should deInit the timer then reInit
	 * it with the same configuration.
	 */
	Timer1_deInit();
	Timer1_ConfigType timer1_configurations = {NORMAL, F_CPU_256, PRELOAD, NOT_CTC};
	Timer1_init(&timer1_configurations);

	/* Decrement the global timer seconds counter */
	timer1_counter--;
}

/* ========================================================================== */
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
void APP_vTimer1CTCCallback(void)
{
	/* Decrement the timer counter until zero, then deinit the timer1 */
	if(--timer1_counter == ZERO)
	{
		Timer1_deInit();
	}
}

/* ========================================================================== */
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
void APP_vUartRxCallback(void)
{
	/* A static varibale to store its value each time the ISR occurs */
	static uint8 frame_index = ZERO;

	/*
	 * Assign each received byte to its corresponding index in RX frame.
	 * It increments the value of frame_index by 1 after its current
	 * value has been used. This means that the value of frame_index
	 * is increased after the assignment operation.
	 */
	g_RX_frame[frame_index++] = UART_recieveByte();

	/* Raise up RX flag when five bytes been completely recieved */
	if(--g_bytes_recieved == ZERO)
	{
		g_RX_ready_flag = TRUE;

		/* Reset the frame index */
		frame_index = ZERO;

		/* Reset to the initial value of number of bytes to recieve */
		g_bytes_recieved = RX_FRAME_SIZE;
	}
}

/* ========================================================================== */
/**
 *	\brief	:	The callback function of INT2 ISR. The two tasks here are to soft
 *				reset the HMI_ECU after reset the M24C16 EEPROM to its initial
 *				value. Its worth to mention that INT2 here is shared with the
 *				INT2 of HMI_ECU; thus, they will reset together.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vINT2softResetCallBack(void)
{
	/* Reset the password state byte */
	EEPROM_writeByte(EEPROM_PASS_STATE_ADDR		, EEPROM_M24C16_INIT_VAL);

	/* Reset the password 4 bytes */
	EEPROM_writeByte(EEPROM_PASS_BYTE1_ADDR+0	, EEPROM_M24C16_INIT_VAL);
	EEPROM_writeByte(EEPROM_PASS_BYTE1_ADDR+1	, EEPROM_M24C16_INIT_VAL);
	EEPROM_writeByte(EEPROM_PASS_BYTE1_ADDR+2	, EEPROM_M24C16_INIT_VAL);
	EEPROM_writeByte(EEPROM_PASS_BYTE1_ADDR+3	, EEPROM_M24C16_INIT_VAL);

	/* Use assembly instruction to jump to RESET address 0 */
	asm volatile("jmp 0");
}
