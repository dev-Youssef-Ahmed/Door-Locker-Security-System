/*******************************************************************************
 *				Final Project	:	Door Locker Security System				   *
 *				File of			:	CONTROL ECU	- main function				   *
 *				Facebook Name	:	Youssef Ahmed							   *
 *				Diploma Number	:	85				 						   *
 ******************************************************************************/

/****************************** GENERAL NOTES *********************************/
/*
 *	Note 1:	MISRA rules were applied to some extent in this project!
 *	Note 2:	The project consists of eleven .c files and thirteen .h files.
 *	Note 3:	Simulation files are attached in "${workspace_loc:/Proteus Simulation".
 *	Note 4:	This project uses Atmega32 with frequency of 8 MHz
 *	Note 5:	Don't forget to assign "inc" folder as the include path to use <main.h>.
 *			Steps:
 * 			I.		Project Properties,
 * 			II.		C/C++ Build,
 * 			III.	Settings,
 * 			IV.		Tool Settings,
 * 			V.		AVR Compiler,
 * 			VI.		Directories,
 * 			VII.	Include Paths (-|): "${workspace_loc:/${ProjName}/inc}".
 *	NOTE 6:	This projec is based on the state-machine algorhitm.
 *	NOTE 7:	Do not miss to check the attached HMI_ECU project.
 *	NOTE 8:	HMI_ECU uses the a main.c code identical to this code, this to
 *			maintain the correct synchronization between the two ECUs.
 */

/************************** HEADER FILES INCLUDES *****************************/

/* Include the necessary header file */
#include <main.h>

/**************************** GLOBAL VARIABLES ********************************/

/*
 * Updated continuously by the functions of application.c. Then the main function
 * will switch on its value to determine the next state to proceed to. The same
 * value will be transmitted to the HMI_ECU through the UART to maintain the sync.
 */
systemStateType g_current_control_state;

/********************** THE STATE-MACHINE ALGORITHM ***************************/

int main (void)
{
	/* Set the initial values and configurations of the application */
	APP_init();

	/* Keep in an infinite loop as long as the system is ON */
	while(SYSTEM_ON)
	{
		/*
		 * Switch on the value of g_current_control_state to determine the desired
		 * next state to proceed with.
		 */
		switch(g_current_control_state)
		{
			/******************************************************************/
			/**************************(FIRST_RUN)*****************************/
			/******************************************************************/

			/*
			 * This state will always be visited at the beginning of running the
			 * program. The next state will navigate into either NEW_PASSWORD
			 * if no password exists, or ENTER_PASSWORD if the password exists.
			 *
			 * In CONTROL_ECU, it will start checking if the password state
			 * indicates the password is existed or not. Then send it to the
			 * HMI_ECU.
			 */
			case FIRST_RUN			:
			{
				APP_vFirstRunState();
				break;
			}

			/******************************************************************/
			/*************************(NEW_PASSWORD)***************************/
			/******************************************************************/

			/*
			 * This state will be visited after one of three states:
			 * 1.	FIRST_RUN, if there is no password exists.
			 * 2.	MISMATCHED_PASSWORD, if the user mismatches the confirmation
			 * 		password for a predefined number of invalid trials.
			 * 3.	MAIN_MENU, if the user chooses to change the password.
			 *
			 * This state will later navigate to the CONFIRM_PASSWORD state.
			 *
			 * Generally, this state will poll until get the password from the
			 * HMI_ECU, then update the g_password with it. At the end, it sends
			 * a control byte to the HMI_ECU to resume the working.
			 */
			case NEW_PASSWORD		:
			{
				APP_vNewPasswordState();
				break;
			}
			/******************************************************************/
			/***********************(CONFIRM_PASSWORD)*************************/
			/******************************************************************/

			/*
			 * This state will be visited after one of two states:
			 * 1.	NEW_PASSWORD, if it was the first time to confirm the enterd
			 * 		new password.
			 * 2.	MISMACHTED_PASSWORD, if the user mismatched the passwords they
			 * 		enter in NEW_PASSWORD and CONFIRM_PASSWORD states for times
			 * 		less than the maximum invalid trials.
			 *
			 * This state will later navigate to the MATCHED_PASSWORD state or
			 * MISMACHTED_PASSWORD state.
			 *
			 * Generally, this state will poll until get the password to be confirmed
			 * from the HMI_ECU, then compare it with the g_password and determine
			 * the next state according to its result. At the end, it sends a
			 * control byte to the HMI_ECU to resume the working.
			 */
			case CONFIRM_PASSWORD	:
			{
				APP_vConfirmNewPasswordState();
				break;
			}

			/******************************************************************/
			/***********************(MATCHED_PASSWORD)*************************/
			/******************************************************************/

			/*
			 * This state will only be visited if the passwords were matched.
			 * It will later navigate to the ENTER_PASSWORD state.
			 *
			 * Generally, this state will poll until ensure that password is saved
			 * correctly into the EEPROM, If a maximum invalid trials to save the
			 * password into the EEPROM exceed, the current state will be repeated.
			 * At the end, it sends a control byte to the HMI_ECU to resume
			 * the working.
			 */
			case MATCHED_PASSWORD	:
			{
				APP_vMatchedPasswordState();
				break;
			}

			/******************************************************************/
			/*********************(MISMACHTED_PASSWORD)************************/
			/******************************************************************/

			/*
			 * This state will only be visited if the passwords were mismatched.
			 * It will later navigate to the CONFIRM_PASSWORD or ENTER_PASSWORD
			 * according to the number of invalid trials.
			 *
			 * Generally, this state will poll until ensure that HMI_ECU have
			 * displayed the mismatching message, then drive the system to the
			 * next state according to the number on mismatching trials, if there
			 * is anymore chance to retry confirming the password, the next state
			 * will be CONFIRM_PASSWORD, otherwise, re-enter a new password. At
			 * the end, it sends a control byte to the HMI_ECU to resume the working.
			 */
			case MISMACHTED_PASSWORD:
			{
				APP_vMisMacthedPasswordState();
				break;
			}

			/******************************************************************/
			/************************(ENTER_PASSWORD)**************************/
			/******************************************************************/

			/*
			 * This state is the most frequently visited state in the system.
			 * It is reached after the following states:
			 * 1.	FIRST_RUN, if the password already exists.
			 * 2.	MATCHED_PASSWORD, if the entered passwords match.
			 * 3.	DOOR_CLOSE, when the door opening process finishes.
			 * 4.	WRONG_PASSWORD, if the entered password is incorrect but the
			 *		maximum number of invalid trials has not been reached.
			 * 5.	LOCK_SYSTEM, when the entered password is incorrect and the
			 *		maximum number of invalid trials has been reached.
			 *
			 * Furthermore, this state can transition to multiple other states:
			 * MAIN_MENU, WRONG_PASSWORD, or LOCK_SYSTEM. The transition is
			 * determined by the CONTROL_ECU based on the condition of the entered
			 * password: correct, incorrect but within the maximum number of
			 * invalid trials, or incorrect and exceeding the maximum number of
			 * invalid trials, respectively.
			 *
			 * Generally, this state will poll until get the password from the
			 * HMI_ECU, then compare it with the password saved in the EEPROM,
			 * then determine the next state according to its result. Definitely,
			 * if the user entered the password correctly, he will navigate to
			 * MAIN_MENU. If not, the next state be determined according to if
			 * there is anymore chance to retry enter the password, if so, the
			 * next state will be WRONG_PASSWORD. Otherwise, the system will be
			 * locked. At the end, it sends a control byte to the HMI_ECU to
			 * resume the working.
			 */
			case ENTER_PASSWORD		:
			{
				APP_vEnterThePasswordState();
				break;
			}

			/******************************************************************/
			/***************************(MAIN_MENU)****************************/
			/******************************************************************/

			/*
			 * This state will only be visited after ENTER_PASSWORD state. It will
			 * later navigate to the NEW_PASSWORD or MAIN_MENU according to the
			 * command of the user.
			 *
			 * Generally, this state will poll until get the command from the
			 * HMI_ECU and switch the current system state accordingly. At the
			 * end, it sends a control byte to the HMI_ECU to resume the working.
			 */
			case MAIN_MENU			:
			{
				APP_vMainMenuState();
				break;
			}

			/******************************************************************/
			/**************************(DOOR_OPEN)*****************************/
			/******************************************************************/

			/*
			 * This state will only be visited after MAIN_MENU state in case the
			 * user selected to open the door. It will later navigate to the state
			 * of DOOR_HOLD.
			 *
			 * Generally, this state will poll until ensure that HMI_ECU have
			 * displayed the door open message, then turn ON the DcMotor for
			 * a certain part of time in CW mode with 100% speed. Then switch
			 * to DOOR_HOLD state. At the end, it sends a control byte to the
			 * HMI_ECU to resume the working.
			 */
			case DOOR_OPEN			:
			{
				APP_vDoorOpenState();
				break;
			}

			/******************************************************************/
			/**************************(DOOR_HOLD)*****************************/
			/******************************************************************/

			/*
			 * This state will only be visited after DOOR_OPEN state. It will
			 * later navigate to the state of DOOR_CLSOE.
			 *
			 * Generally, this state will poll until ensure that HMI_ECU have
			 * displayed the door hold message, then turn OFF the DcMotor for
			 * a certain part of time. Then switch to DOOR_CLOSE state. At the
			 * end, it sends a control byte to the HMI_ECU to resume the working.
			 */
			case DOOR_HOLD			:
			{
				APP_vDoorHoldState();
				break;
			}

			/******************************************************************/
			/**************************(DOOR_CLOSE)****************************/
			/******************************************************************/

			/*
			 * This state will only be visited after DOOR_HOLD state. It will
			 * later navigate to the state of ENTER_PASSWORD.
			 *
			 * Generally, this state will poll until ensure that HMI_ECU have
			 * displayed the door close message, then turn ON the DcMotor for
			 * a certain part of time in A-CW mode with 100% speed. Then switch
			 * to ENTER_PASSWORD state. At the end, it sends a control byte to the
			 * HMI_ECU to resume the working.
			 */
			case DOOR_CLOSE			:
			{
				APP_vDoorCloseState();
				break;
			}

			/******************************************************************/
			/************************(WRONG_PASSWORD)**************************/
			/******************************************************************/

			/*
			 * This state will only be visited after ENTER_PASSWORD state in case
			 * of the entered password is incorrect but the maximum number of
			 * invalid trials has not been reached. It will later repeat the state
			 * of ENTER_PASSWORD.
			 *
			 * Generally, this state will poll until ensure that HMI_ECU have
			 * displayed the wrong password message, then switch to ENTER_PASSWORD
			 * state. At the end, it sends a control byte to the HMI_ECU to resume
			 * the working.
			 */
			case WRONG_PASSWORD		:
			{
				APP_vWrongPasswordState();
				break;
			}

			/******************************************************************/
			/**************************(LOCK_SYSTEM)***************************/
			/******************************************************************/

			/*
			 * This state will only be visited after ENTER_PASSWORD state in case
			 * of the entered password is incorrect and the maximum number of
			 * invalid trials has been reached. It will later repeat the state
			 * of ENTER_PASSWORD.
			 *
			 * Generally, this state will poll until ensure that HMI_ECU have
			 * displayed the wrong password message, Then buzzer will be turned
			 * ON for a certain part of time. Finally, switch to ENTER_PASSWORD
			 * state. At the end, it sends a control byte to the HMI_ECU to resume
			 * the working.
			 */
			case LOCK_SYSTEM		:
			{
				APP_vLockSystemState();
				break;
			}
		}
	}
}
