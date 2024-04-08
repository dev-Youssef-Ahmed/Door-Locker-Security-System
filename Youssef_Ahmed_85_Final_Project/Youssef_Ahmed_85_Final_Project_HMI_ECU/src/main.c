/*******************************************************************************
 *				Final Project	:	Door Locker Security System				   *
 *				File of			:	HMI ECU	- main function					   *
 *				Facebook Name	:	Youssef Ahmed							   *
 *				Diploma Number	:	85				 						   *
 ******************************************************************************/

/****************************** GENERAL NOTES *********************************/
/*
 *	Note 1:	MISRA rules were applied to some extent in this project!
 *	Note 2:	The project consists of eight .c files and twelve .h files.
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
 *	NOTE 7:	Do not miss to check the attached CONTROL_ECU project.
 *	NOTE 8:	CONTROL_ECU uses the a main.c code identical to this code, this to
 *			maintain the correct synchronization between the two ECUs.
 */

/************************** HEADER FILES INCLUDES *****************************/

/* Include the necessary header file */
#include <main.h>

/**************************** GLOBAL VARIABLES ********************************/

/*
 * Updated continuously by the functions of application.c. Then the main function
 * will switch on its value to determine the next state to proceed to. Its value
 * will be assigned according to the state that CONTROL_ECU will send to the HMI
 * through the UART control byte (byte 0 of each received 5 bytes in the RX frame).
 */
systemStateType g_current_hmi_state;

/********************** THE STATE-MACHINE ALGORITHM ***************************/

int main (void)
{
	/* Set the initial values and configurations of the application */
	APP_init();

	/* Keep in an infinite loop as long as the system is ON */
	while(SYSTEM_ON)
	{
		/*
		 * Switch on the value of g_current_hmi_state to determine the desired
		 * next state to proceed with.
		 */
		switch(g_current_hmi_state)
		{
			/******************************************************************/
			/**************************(FIRST_RUN)*****************************/
			/******************************************************************/

			/*
			 * This state will always be visited at the beginning of running the
			 * program. The next state will navigate into either NEW_PASSWORD
			 * if no password exists, or ENTER_PASSWORD if the password exists.
			 *
			 * In HMI_ECU, it will poll until it receives the next state from
			 * CONTROL_ECU. If the next state is NEW_PASSWORD, the LCD will
			 * display welcome messages to the user before navigating to the
			 * NEW_PASSWORD state.
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
			 * Generally, this state will display two LCD messages asking the
			 * user for the password after informing them of the password rules
			 * to enter. After that, the HMI_ECU will send this password to the
			 * CONTROL_ECU and then poll until it receives the desired new state
			 * from the CONTROL_ECU.
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
			 * Generally, this state will display a LCD message asking the user
			 * for confirmation of their new password. After that, the HMI_ECU
			 * will send this password to the CONTROL_ECU and then poll until it
			 * receives the desired new state from the CONTROL_ECU.
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
			 * Generally, this state will display an LCD message informing the
			 * user that their passwords were matched. After that, it will poll
			 * until receiving the desired new state from the CONTROL_ECU.
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
			 * Generally, this state will display an LCD message informing the
			 * user that their passwords were mismatched. After that, it will
			 * poll until receiving the desired new state from the CONTROL_ECU.
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
			 * Generally, this state will display an LCD message prompting the
			 * user to enter their password. After that, the HMI_ECU will send
			 * this password to the CONTROL_ECU and then poll until it receives
			 * the desired new state from the CONTROL_ECU.
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
			 * later navigate to the DOOR_OPEN or NEW_PASSWORD according to the
			 * command of the user.
			 *
			 * Generally, this state will display an LCD message asking the user
			 * to enter their desired command, either to open the door (+), or to
			 * change the password (-). After that, it will poll until receiving
			 * the desired new state from the CONTROL_ECU.
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
			 * Generally, this state will display an LCD message informing the
			 * user that the door is openning for a certain part of time. After
			 * that, it will poll until receiving the desired new state from the
			 * CONTROL_ECU.
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
			 * Generally, this state will display an LCD message informing the
			 * user that the door is holding for a certain part of time. After
			 * that, it will poll until receiving the desired new state from the
			 * CONTROL_ECU.
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
			 * Generally, this state will display an LCD message informing the
			 * user that the door is closing for a certain part of time. After
			 * that, it will poll until receiving the desired new state from the
			 * CONTROL_ECU.
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
			 * Generally, this state will display an LCD message informing the
			 * user that their password is wrong. After that, it will poll until
			 * receiving the desired new state from the CONTROL_ECU.
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
			 * Generally, this state will display an LCD message informing the
			 * user that their trials are suspected, and the system will be closed
			 * for a certain part of time. After that, it will poll until receiving
			 * the desired new state from the CONTROL_ECU.
			 */
			case LOCK_SYSTEM		:
			{
				APP_vLockSystemState();
				break;
			}
		}
	}
}
