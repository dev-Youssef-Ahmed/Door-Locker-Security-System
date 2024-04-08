/*******************************************************************************
 *				Final Project	:	Door Locker Security System				   *
 *				File of			:	HMI ECU	- application functions			   *
 *				Facebook Name	:	Youssef Ahmed							   *
 *				Diploma Number	:	85				 						   *
 ******************************************************************************/

/************************************************************
 * \note: set your timer1 mode and times (in seconds) here!	*
 * \note: JUST COPY YOUR DESIRED VALUE FROM THE				*
 * 		  COMMENT UPSIDE EACH FOLLOWING #define 			*
 ***********************************************************/

#warning: DO NOT FORGET TO SYNC THE TIME OF CONTROL_ECU/src/application.c

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

/* Holds the number of received bytes, decrements from 1 to 0 */
uint8	g_bytes_recieved;

/* Holds TRUE if 2 bytes are received, then changes to FALSE */
boolean	g_RX_ready_flag;

/*
 * Holds the received  2 bytes:
 *		[Byte0: Success or Fail] [Byte1: CTRL]
 */
uint8	g_RX_frame[RX_FRAME_SIZE];

/*
 * Holds transmittable 5 bytes:
 *
 * In case of transmitting the uint32 g_password:
 * 		[Byte0: CTRL] [Byte1: Password Byte 0] [Byte2: Password Byte 1]
 * 		[Byte3: Password Byte 2] [Byte4: Password Byte 3]
 *
 * In case of transmitting user's command:
 * 		[Byte0: CTRL] [Byte1: Command] [Bytes 2-4: EMPTY]
 *
 * In case of transmitting the current state:
 * 		[Byte0: CTRL] [Bytes 1-4: EMPTY]
 */
uint8	g_TX_frame[TX_FRAME_SIZE];

/* Holds the password that will be transmitted to CONTROL_ECU */
uint32	g_password;

/**************************** APP INITIALIZATION ******************************/

/**
 *	\brief	:	Iniitiate the application.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_init(void)
{
	/* Enter the first run system state, which will determine the following steps */
	g_current_hmi_state = FIRST_RUN;

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
	/* Select the callback functions that will be called in any of Timer1 ISRs occurrences */
	Timer1_setCallBack(&TimerCallback);

	/* ====================================================================== */
	/*						 Interrupt 2 configurations:					  */
	/* ====================================================================== */

	/* Initiate the parameters of INT2, which is responsible for RESET process */
	INT2_init();
	/* Select the callback function that will be called in INT2 ISR occurrences */
	INT2_setCallBack(APP_vINT2softResetCallBack);

	/* ====================================================================== */
	/* 							UART configurations:						  */
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

	/* ====================================================================== */
	/* 						Components initializations:						  */
	/* ====================================================================== */

	/* GPIO is a static configurable in lcd.h */
	LCD_init();

	/* ====================================================================== */
	/*							Enable global interrupt						  */
	/* ====================================================================== */
	sei();
}

/************************ SYSTEM STATES FUNCTIONS *****************************/

/* ========================================================================== */
/**
 *	\brief	:	Display the welcome message if CONTROL_ECU's CTRL byte was matched.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = FIRST_RUN.
 **/
void APP_vFirstRunState(void)
{
	/*
	 * =========================================================================
	 * NOTE: Check APP_vCheckSuccessSyncTwoStates function description first.
	 * =========================================================================
	 * Set system state to NEW_PASSWORD state if CTRL byte referred to, otherwise,
	 * set it to ENTER_PASSWORD state.
	 *
	 * See APP_vFirstRunState in CONTROL_ECU to understand the mechanism of deciding
	 * the next state after FIRST_RUN state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 * =========================================================================
	 */
	APP_vCheckSuccessSyncTwoStates(CTRL_GET_NEW_PASSWORD, NEW_PASSWORD, ENTER_PASSWORD, FIRST_RUN);

	/*
	 * =========================================================================
	 * NOTE: Check APP_displayMessage function description first.
	 * =========================================================================
	 * Only in case if it was really the 1st time to run the program, display
	 * these messages on the LCD.
	 * =========================================================================
	 */
	if (g_current_hmi_state == NEW_PASSWORD)
	{
		/*LCD: Hi! This is Your First Time Here! It's a Door Lock Security System. */
		APP_displayMessage(MSG_FIRST_RUN);

		/* Start timer1 seconds counter to make the message readable */
		TIMER1_vStartTimer(TIMER1_DOOR_HOLD_AND_LCD_MESSAGES);
	}
}

/* ========================================================================== */
/**
 *	\brief	:	Display messages in case of set a new password,
 *				whether in 1st time or in change password state.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = NEW_PASSWORD.
 **/
void APP_vNewPasswordState(void)
{
	/*LCD: ** Your Password Should be Ranged in (10000-99999) and be 5-digits. */
	APP_displayMessage(MSG_PSWRD_INFO);

	/* Start timer1 seconds counter to make the message readable */
	TIMER1_vStartTimer(TIMER1_DOOR_HOLD_AND_LCD_MESSAGES);

	/*LCD: ** Here We Go ** Now, Use the Key Pad to Enter the Password:        */
	APP_displayMessage(MSG_NEW_PSWRD);

	/*
	 * =========================================================================
	 * NOTE: Check APP_vGetPasswordFromUserThenSendUART function description first.
	 * =========================================================================
	 * Keep polling until get an accepted password from the user, then send it
	 * to the CONTROL_ECU though UART with new password CTRL byte.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 * =========================================================================
	 */
	APP_vGetPasswordFromUserThenSendUART(CTRL_GET_NEW_PASSWORD);

	/*
	 * =========================================================================
	 * NOTE: Check APP_vCheckSuccessSyncOneState function description first.
	 * =========================================================================
	 * Set system state to CONFIRM_PASSWORD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 * =========================================================================
	 */
	APP_vCheckSuccessSyncOneState(CONFIRM_PASSWORD, NEW_PASSWORD);
}

/* ========================================================================== */
/**
 *	\brief	:	Display messages to confirm the new password.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = CONFIRM_PASSWORD.
 **/
void APP_vConfirmNewPasswordState(void)
{
	/*LCD:   Now, confirm    Your Password!                   Password:        */
	APP_displayMessage(MSG_CFRM_PSWRD);

	/*
	 * Keep polling until get an accepted password from the user, then send it
	 * to the CONTROL_ECU though UART with confirm new password CTRL byte.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 */
	APP_vGetPasswordFromUserThenSendUART(CTRL_CONFIRM_NEW_PASSWORD);

	/*
	 * Set system state to MATCHED_PASSWORD state if the received CTRL byte referred to,
	 * otherwise, set it to MISMACHTED_PASSWORD state.
	 *
	 * See APP_vConfirmNewPasswordState in CONTROL_ECU to understand the mechanism
	 * of deciding the next state after CONFIRM_PASSWORD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncTwoStates(CTRL_PASSWORD_MATCHED, MATCHED_PASSWORD, MISMACHTED_PASSWORD, CONFIRM_PASSWORD);
}

/* ========================================================================== */
/**
 *	\brief	:	Display messages to confirm that your passwords matched.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MATCHED_PASSWORD.
 **/
void APP_vMatchedPasswordState(void)
{
	/* LCD: ** Succussed ***   Your door is     Secured with     the password   */
	APP_displayMessage(MSG_MTCH_PSWRD);

	/* Start timer1 seconds counter to make the message readable */
	TIMER1_vStartTimer(TIMER1_DOOR_HOLD_AND_LCD_MESSAGES);
	/*
	 * Set system state to ENTER_PASSWORD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncOneState(ENTER_PASSWORD, MATCHED_PASSWORD);
}

/* ========================================================================== */
/**
 *	\brief	:	Display messages to inform the user that the password and its
 *				confirmation are not matched. If mismatched 3 times, redo the
 *				enter new password state. If the passwords are matched, we will
 *				not navigate to this state.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MISMACHTED_PASSWORD.
 **/
void APP_vMisMacthedPasswordState(void)
{
	/* LCD: ***  Failed  *** Passwords aren't Matching! Retype The Password ... */
	APP_displayMessage(MSG_MSMT_PSWRD);

	/* Start timer1 seconds counter to make the message readable */
	TIMER1_vStartTimer(TIMER1_DOOR_HOLD_AND_LCD_MESSAGES);

	/*
	 * =========================================================================
	 * NOTE: Check APP_vSendCurrentHMIStateUART function description first.
	 * =========================================================================
	 * Send a CTRL byte to inform the CONTROL_ECU that mismatching message displayed.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 * =========================================================================
	 */
	APP_vSendCurrentHMIStateUART(CTRL_PASSWORD_MISMATCHED);

	/*
	 * Set system state to CONFIRM_PASSWORD state if the received CTRL byte referred to,
	 * otherwise, set it to NEW_PASSWORD state.
	 *
	 * See APP_vMisMacthedPasswordState in CONTROL_ECU to understand the mechanism
	 * of deciding the next state after MISMACHTED_PASSWORD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncTwoStates(CTRL_CONFIRM_NEW_PASSWORD, CONFIRM_PASSWORD, NEW_PASSWORD, MISMACHTED_PASSWORD);
}

/* ========================================================================== */
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
void APP_vEnterThePasswordState(void)
{
	/* LCD: Welcome Back! To Open the Door,   Please Enter the Password:        */
	APP_displayMessage(MSG_ENTR_PSWRD);

	/*
	 * Keep polling until get an accepted password from the user, then send it
	 * to the CONTROL_ECU though UART with enter password CTRL byte.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 */
	APP_vGetPasswordFromUserThenSendUART(CTRL_ENTER_PASSWORD);

	/*
	 * =========================================================================
	 * NOTE: Check APP_vCheckSuccessSyncThreeStates function description first.
	 * =========================================================================
	 * Set system state to MAIN_MENU state if CTRL byte referred to, or to WRONG_PASSWORD
	 * if CTRL byte referred to, otherwise, set it to LOCK_SYSTEM state.
	 *
	 * See APP_vEnterThePasswordState in CONTROL_ECU to understand the mechanism
	 * of deciding the next state after ENTER_PASSWORD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 * =========================================================================
	 */
	APP_vCheckSuccessSyncThreeStates(CTRL_MAIN_MENU, CTRL_WRONG_PASSWORD,\
					MAIN_MENU, WRONG_PASSWORD, LOCK_SYSTEM, ENTER_PASSWORD);
}

/* ========================================================================== */
/**
 *	\brief	:	Display messages to ask for the desired command, whether to open
 *				the door or to change the current password and set a new password.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = MAIN_MENU.
 **/
void APP_vMainMenuState(void)
{
	/* LCD: Press ( + ) to:  Open the Door    Press ( - ) to:  Change Password  */
	APP_displayMessage(MSG_MAIN_MENU);

	/*
	 * =========================================================================
	 * NOTE: Check APP_vGetCommandFromUserThenSendUART function description first.
	 * =========================================================================
	 * Keep polling until get an accepted command from the user, then send it
	 * to the CONTROL_ECU though UART with main menu CTRL byte.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 * =========================================================================
	 */
	APP_vGetCommandFromUserThenSendUART();

	/*
	 * Set system state to DOOR_OPEN state if the received CTRL byte referred to,
	 * otherwise, set it to NEW_PASSWORD state.
	 *
	 * See APP_vMainMenuState in CONTROL_ECU to understand the mechanism
	 * of deciding the next state after MAIN_MENU state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncTwoStates(CTRL_DOOR_OPEN, DOOR_OPEN, NEW_PASSWORD, MAIN_MENU);
}

/* ========================================================================== */
/**
 *	\brief	:	Display message that the door is openning now. We will navigate
 *				here only if the user choose to open the door.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_OPEN.
 **/
void APP_vDoorOpenState(void)
{
	/* LCD:  Sure, The Door   Is Opening Now   for 15 Seconds                   */
	APP_displayMessage(MSG_DOOR_OPEN);

	/*
	 * Send a CTRL byte to inform the CONTROL_ECU that door open message displayed.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 */
	APP_vSendCurrentHMIStateUART(CTRL_DOOR_OPEN);

	/*
	 * Set system state to DOOR_HOLD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncOneState(DOOR_HOLD, DOOR_OPEN);
}

/* ========================================================================== */
/**
 *	\brief	:	Display message that the door is holding now. We will navigate here
 *				only after door been completely open
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_HOLD.
 **/
void APP_vDoorHoldState(void)
{
	/* LCD:  *The Door Will   Keep Open Mode    for 3 Seconds      Hurry Up!    */
	APP_displayMessage(MSG_DOOR_HOLD);

	/*
	 * Send a CTRL byte to inform the CONTROL_ECU that door hold message displayed.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 */
	APP_vSendCurrentHMIStateUART(CTRL_DOOR_HOLD);

	/*
	 * Set system state to DOOR_HOLD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncOneState(DOOR_CLOSE, DOOR_HOLD);
}

/* ========================================================================== */
/**
 *	\brief	:	Display message that the door is holding now. We will navigate here
 *				only after door been completely open
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = DOOR_CLOSE.
 **/
void APP_vDoorCloseState(void)
{
	/* LCD:  Oops, The Door   is Closing Now   for 15 Seconds                   */
	APP_displayMessage(MSG_DOOR_CLOS);

	/*
	 * Send a CTRL byte to inform the CONTROL_ECU that door close message displayed.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 */
	APP_vSendCurrentHMIStateUART(CTRL_DOOR_CLOSE);

	/*
	 * Re-set system state to ENTER_PASSWORD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncOneState(ENTER_PASSWORD, DOOR_CLOSE);
}

/* ========================================================================== */
/**
 *	\brief	:	Display message that the enterd password was wrong, and it will
 *				be visited only in case of less than maximum number of invalid
 *				trials satisfied. Then re-set the state to enter password state.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = WRONG_PASSWORD.
 **/
void APP_vWrongPasswordState(void)
{
	/* LCD: Invalid Password Please Focus and Enter the Right  Password Again ! */
	APP_displayMessage(MSG_WRNG_PSWRD);

	/* Start timer1 seconds counter to make the message readable */
	TIMER1_vStartTimer(TIMER1_DOOR_HOLD_AND_LCD_MESSAGES);

	/*
	 * Send a CTRL byte to inform the CONTROL_ECU that wrong password message displayed.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 */
	APP_vSendCurrentHMIStateUART(CTRL_WRONG_PASSWORD);

	/*
	 * Set system state to ENTER_PASSWORD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncOneState(ENTER_PASSWORD, WRONG_PASSWORD);
}

/* ========================================================================== */
/**
 *	\brief	:	Display message that the system is locked for 1 minute. We will
 *				navigate here only if reached the maximum number of password's
 *				invalid trials. After lock the system, return to enter password
 *				state.
 *	\param	:	void.
 *	\return	:	void.
 *	\note	: 	Now in current_system_state = LOCK_SYSTEM.
 **/
void APP_vLockSystemState(void)
{
	/* LCD: Suspected Trials ================ The Door will be Locked for 1 Min */
	APP_displayMessage(MSG_LOCK_SYS);

	/*
	 * Send a CTRL byte to inform the CONTROL_ECU that system locked message displayed.
	 *
	 * Note that unless this function be completed, the CONTROL_ECU is in
	 * polling state awaiting for the CTRL byte from the HMI.
	 */
	APP_vSendCurrentHMIStateUART(CTRL_LOCK_SYSTEM);

	/*
	 * Re-set system state to ENTER_PASSWORD state.
	 *
	 * If ERROR byte received, redo this state to keep synced with CONTROL_ECU state.
	 */
	APP_vCheckSuccessSyncOneState(ENTER_PASSWORD, LOCK_SYSTEM);
}

/*********** APP DIRECT-SUPPORT FUNCTIONS: Sync Control Functions *************/

/* ========================================================================== */
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
void APP_vCheckSuccessSyncOneState(systemStateType_ if_success, systemStateType_ if_fail)
{
	/* Polling until receive a UART frame */
	while(g_RX_ready_flag == FALSE);

	/* Assign the next state according to CTRL byte0 value, to go, or to repeat! */
	g_current_hmi_state = ((g_RX_frame[BYTE_0_CONTROL] == CTRL_SUCCESS) ? if_success : if_fail);

	/* Reset the RX flag to be reusable in the next process */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
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
										systemStateType_		if_fail				)
{
	/* Polling until receive a UART frame */
	while(g_RX_ready_flag == FALSE);

	/* Check the CTRL byte0 to change the state if successed, or to repeat if error */
	if (g_RX_frame[BYTE_0_CONTROL] == CTRL_SUCCESS)
	{
		/* Assign the next state according to CTRL byte1 value as desciped in \brief */
		g_current_hmi_state = ((g_RX_frame[BYTE_1_PASS0_OR_CMD] == CTRL_if_success1) ?	if_success1 :\
																						if_success2);
	}
	/* Repeat the state if CTRL byte0 is error */
	else
	{
		g_current_hmi_state = if_fail;
	}

	/* Reset the RX flag to be reusable in the next process */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
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
										systemStateType_		if_fail				)
{
	/* Polling until receive a UART frame */
	while(g_RX_ready_flag == FALSE);

	/* Check the CTRL byte0 to change the state if successed, or to repeat if error */
	if (g_RX_frame[BYTE_0_CONTROL] == CTRL_SUCCESS)
	{
		/* Assign the next state according to CTRL byte1 value as desciped in \brief */
		g_current_hmi_state =	((g_RX_frame[BYTE_1_PASS0_OR_CMD] == CTRL_if_success1) ? if_success1 :\
								((g_RX_frame[BYTE_1_PASS0_OR_CMD] == CTRL_if_success2) ? if_success2 :\
																						if_success3));
	}
	/* Repeat the state if CTRL byte0 is error */
	else
	{
		g_current_hmi_state = if_fail;
	}

	/* Reset the RX flag to be reusable in the next process */
	g_RX_ready_flag = FALSE;
}

/* ========================================================================== */
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
void APP_vSendCurrentHMIStateUART(frameType byte_0)
{
	/* Check if the entered current state is main menu */
	switch(byte_0)
	{
		/*
		 * =========================================================================
		 * NOTE: Check APP_vGetCommandFromUserThenSendUART function description first.
		 * =========================================================================
		 * Use the independent function of handling user command.
		 * =========================================================================
		 */
		case CTRL_MAIN_MENU	:
		{
			APP_vGetCommandFromUserThenSendUART();
			break;
		}
		/*
		 * =========================================================================
		 * NOTE: Check APP_vUpdateTXFrameThenSendUART function description first.
		 * =========================================================================
		 * At any other state, just update the TX frame with the current CTRL byte.
		 * =========================================================================
		 */
		default				:
		{
			APP_vUpdateTXFrameThenSendUART(byte_0, CTRL_EMPTY, CTRL_EMPTY, CTRL_EMPTY, CTRL_EMPTY);
			break;
		}
	}
}

/****** APP DIRECT-SUPPORT FUNCTIONS: User Input Acquisition Functions ********/

/* ========================================================================== */
/**
 *	\brief	:	Calls the function of getting user password, then separate it from
 *				four byte (sizeof(uint32) = 4 bytes) into four different bytes.
 *				After updating the TX frame byte-by-byte, use UART send TX frame
 *				function to transmit this updated frame.
 *	\param 1:	CTRL byte that indicates the current state.
 *	\return	:	void.
 **/
void APP_vGetPasswordFromUserThenSendUART(UART_ControlByteType control_byte)
{
	/* Used as for loop incremental parameter */
	frameType	byte_index;
	/* Holds the current value of each byte */
	uint8		byte;

	/*
	 * =========================================================================
	 * NOTE: Check APP_vGetPasswordFromUser function description first.
	 * =========================================================================
	 * Polling until get a valid uin32 password from the user.
	 * =========================================================================
	 */
	APP_vGetPasswordFromUser();

	/* Update byte0 of TX frame with a CTRL byte indicates the current system state. */
	g_TX_frame[BYTE_0_CONTROL] = control_byte;

	/* Update bytes 1 to 5 with the 4 separated bytes of the uint32 g_password */
	for(byte_index = BYTE_1_PASS0_OR_CMD ; byte_index <= BYTE_4_PASS3 ; byte_index++)
	{
		/*
		 * The bytes are extracted from a password using bitwise operations by
		 * shifting the password to the right by a calculated number of bits,
		 * which is determined by multiplying the byte index by the number of
		 * bits per byte. This operation isolates each byte within the password.
		 */
		byte = (uint8)(g_password >> ((BYTES_PER_UINT32 - byte_index) * BITS_PER_BYTE));

		/* Assign bytes to frame */
		g_TX_frame[byte_index] = byte;
	}

	/*
	 * =========================================================================
	 * NOTE: Check UART_sendGolabalFrame function description first.
	 * =========================================================================
	 * TX frame is ready now, lets transmit it!
	 * =========================================================================
	 */
	UART_sendGolabalFrame(g_TX_frame, TX_FRAME_SIZE);
}

/* ========================================================================== */
/**
 *	\brief	:	Calls the function of getting user password, then update the TX
 *				frame and send it, including the desired user command.
 *	\param :	void.
 *	\return	:	void.
 **/
void APP_vGetCommandFromUserThenSendUART(void)
{
	/*
	 * =========================================================================
	 * NOTE: Check APP_u8GetCommandFromUser function description first.
	 * =========================================================================
	 * Polling until get a valid command from the user, then assign it to a temp
	 * variable.
	 * =========================================================================
	 */
	uint8 user_command = APP_u8GetCommandFromUser();

	/*
	 * Update the TX frame with CTRL main menu byte0, and the assigned variable
	 * as byte1, then fill the TX frame with EMPTYs since there is no more data
	 * to transmit, then send the whole frame.
	 */
	APP_vUpdateTXFrameThenSendUART(CTRL_MAIN_MENU, user_command,\
							CTRL_EMPTY, CTRL_EMPTY, CTRL_EMPTY);
}

/************ APP DIRECT-SUPPORT FUNCTIONS: LCD Messages Functions ************/

/* ========================================================================== */
/**
 *	\brief	:	Displays a predefined messages according to the given message ID.
 *				This function is the beginning of each system state function (except
 *				the APP_vFirstRunState function) to make sure that the user will not
 *				keep awaiting the CONTROL_ECU decisions with an empty screen!
 *	\param 1:	The desired message ID to display on the LCD.
 *	\return	:	void.
 **/
void APP_displayMessage(messagesType message_ID)
{
	/* Maintain writing on a clear screen every time this function been called. */
	LCD_clearScreen();

	/*
	 * Switch on the required message, each message is corresponding to a single
	 * system state. For each message, move cursor to the beginning of each line
	 * then write the desired line within this message.
	 */
	switch(message_ID)
	{
		case MSG_FIRST_RUN:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("Hi! This is Your");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("First Time Here!");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("It's a Door Lock");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("Security System.");
			break;
		}

		case MSG_PSWRD_INFO:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("** Your Password");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("Should be Ranged");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("in (10000-99999)");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("and be 5-digits.");
			break;
		}

		case MSG_NEW_PSWRD:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("** Here We Go **");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("Now, Use the Key");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("Pad to Enter the");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("Password:       ");
			break;
		}

		case MSG_CFRM_PSWRD:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("  Now, confirm  ");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString(" Your Password! ");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("                ");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("Password:       ");
			break;
		}

		case MSG_MTCH_PSWRD:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("** Succussed ***");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("  Your door is  ");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("  Secured with  ");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("  the password  ");
			break;
		}

		case MSG_MSMT_PSWRD:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("***  Failed  ***");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("Passwords aren't");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("Matching! Retype");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("The Password ...");
			break;
		}

		case MSG_ENTR_PSWRD:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("Welcome Back! To");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("Open the Door,  ");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("Please Enter the");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("Password:       ");
			break;
		}

		case MSG_MAIN_MENU:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("Press ( + ) to: ");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("Open the Door   ");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("Press ( - ) to: ");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("Change Password ");
			break;
		}

		case MSG_DOOR_OPEN:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString(" Sure, The Door ");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString(" Is Opening Now ");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString(" for 15 Seconds ");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("                ");
			break;
		}

		case MSG_DOOR_HOLD:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString(" *The Door Will ");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString(" Keep Open Mode ");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("  for 3 Seconds ");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("    Hurry Up!   ");
			break;
		}

		case MSG_DOOR_CLOS:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString(" Oops, The Door ");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString(" is Closing Now ");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString(" for 15 Seconds ");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("                ");
			break;
		}

		case MSG_WRNG_PSWRD:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("Invalid Password");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("Please Focus and");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("Enter the Right ");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("Password Again !");
			break;
		}

		case MSG_LOCK_SYS:
		{
			LCD_moveCursor(ROW_0, COL_0);
			LCD_displayString("Suspected Trials");
			LCD_moveCursor(ROW_1, COL_0);
			LCD_displayString("================");
			LCD_moveCursor(ROW_2, COL_0);
			LCD_displayString("The Door will be");
			LCD_moveCursor(ROW_3, COL_0);
			LCD_displayString("Locked for 1 Min");
			break;
		}
	}
}

/****** APP INDIRECT-SUPPORT FUNCTIONS: Handling User's inputs Functions ******/

/* ========================================================================== */
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
void APP_vGetPasswordFromUser(void)
{
	/* Holds the pressed key acquired by KEYPAD driver */
	KEYPAD_numbersType	pressed_key;
	/* Used as index of for loop */
	passwordDigitsType	password_digit;

	/* Assign a zero initial value to the g_password */
	g_password = ZERO;

	/*
	 * For loop of five iteraations to get a 5 digit password from the user. See
	 * the discription of the "if condition TRUE case" to understand why the
	 * index started from 1 not 0.
	 */
	for(password_digit = DIGIT_1 ; password_digit <= DIGIT_5 ; password_digit++)
	{
		/* Use KEYPAD driver to get the user input (Polling) */
		pressed_key = KEYPAD_getPressedKey();

		/* Validating user's input, if rules were violated: */
		if(	(password_digit == DIGIT_1 	&& pressed_key == KP_ZERO)	||\
			(pressed_key	!= KP_ZERO	&& pressed_key != KP_ONE	&&\
			 pressed_key	!= KP_TWO	&& pressed_key != KP_THREE	&&\
			 pressed_key	!= KP_FOUR	&& pressed_key != KP_FIVE	&&\
			 pressed_key	!= KP_SIX	&& pressed_key != KP_SEVEN	&&\
			 pressed_key	!= KP_EIGHT	&& pressed_key != KP_NINE)	)
		{
			/*
			 * If the user violated the password rules, undo this iteration. In
			 * case if it was the 1st digit, and if we started from digit0, then
			 * if the 0 decremented by 1, and its an unsigned varibale, the value
			 * of the pressed key will be overflowed! Alternatively, we can start
			 * from digit1 to avoid this case, so if digit1 rules have violated,
			 * then it will decremented to 0, and quits this for loop iteration.
			 * When it quit the for loop, it will be automatically incremented by
			 * one (the increment factor of the for loop) so it will return to the
			 * real current digit.
			 */
			password_digit--;
		}
		/* If correct digit entered: */
		else
		{
			/* Switch on the digit to update the g_password as described in \brief */
			switch(password_digit)
			{
				case DIGIT_0:
				{
					/* DO NOTHING, anyway it will be incremented and restart from DIGIT_1 */
					break;
				}
				case DIGIT_1:
				{
					g_password  =  pressed_key * MULTIPLIER_10000;
					break;
				}

				case DIGIT_2:
				{
					g_password +=  pressed_key * MULTIPLIER_1000;
					break;
				}

				case DIGIT_3:
				{
					g_password +=  pressed_key * MULTIPLIER_100;
					break;
				}

				case DIGIT_4:
				{
					g_password +=  pressed_key * MULTIPLIER_10;
					break;
				}

				case DIGIT_5:
				{
					g_password +=  pressed_key * MULTIPLIER_1;
					break;
				}
			}

			/*
			 * The password always have the same LCD location, a single row and
			 * a continous columns. Thus, we can simply display the asterisk and
			 * the entered digit on the 1st digit column location plus the current
			 * digit index as described in \brief.
			 */
			LCD_moveCursor(PASSWORD_ROW, (PASSWORD_COL + password_digit));
			LCD_intgerToString(pressed_key);
			DELAY_FIFTY_MS;
			LCD_moveCursor(PASSWORD_ROW, (PASSWORD_COL + password_digit));
			LCD_displayCharacter('*');
		}
	}
}

/* ========================================================================== */
/**
 *	\brief	:	Get only accepted command form user (+ to open the door, and - to
 *				change the password) and return this command to be used later.
 *	\param 	:	void.
 *	\return	:	The entered command by user.
 **/
uint8 APP_u8GetCommandFromUser(void)
{
	/* Holds the command of the user, and will be returned */
	uint8 user_command;

	/* Polling until get a valid input only using do while loop */
	do
	{
		user_command = KEYPAD_getPressedKey();
	} while ((user_command != CMD_OPN_DOOR) && (user_command != CMD_CHNG_PSWRD));

	/* Return the command */
	return user_command;
}

/****** APP INDIRECT-SUPPORT FUNCTIONS: Handling UART TX Frame Functions ******/

/* ========================================================================== */
/**
 *	\brief		:	Get the TX frame elemtns and assign them to the desired array
 *					location, then send them all.
 *	\param 1-5	:	The elements of TX frame.
 *	\return		:	void.
 **/
void APP_vUpdateTXFrameThenSendUART(frameType byte_0, frameType byte_1, frameType byte_2,\
									frameType byte_3, frameType byte_4)
{
    g_TX_frame[BYTE_0_CONTROL]		= byte_0;
    g_TX_frame[BYTE_1_PASS0_OR_CMD]	= byte_1;
    g_TX_frame[BYTE_2_PASS1]		= byte_2;
    g_TX_frame[BYTE_3_PASS2]		= byte_3;
    g_TX_frame[BYTE_4_PASS3]		= byte_4;

    UART_sendGolabalFrame(g_TX_frame, TX_FRAME_SIZE);
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
 *	\brief	:	The callback function of INT2 ISR. The only task here is to soft
 *				reset the HMI_ECU. Its worth to mention that INT2 here is shared
 *				with the INT2 of CONTROL_ECU; thus, they will reset together.
 *	\param	:	void.
 *	\return	:	void.
 **/
void APP_vINT2softResetCallBack(void)
{
	/* Use assembly instruction to jump to RESET address 0 */
	asm volatile("jmp 0");
}
