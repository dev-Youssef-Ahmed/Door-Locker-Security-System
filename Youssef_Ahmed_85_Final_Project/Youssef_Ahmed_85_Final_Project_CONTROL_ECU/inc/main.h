/************************ PREPROCESSOR HEADER GUARD ***************************/
/*
 * 	A preprocessor directive used to prevent a header file from being included
 * 	multiple times in the same compilation unit, which could lead to issues like
 * 	redefinition errors.
 */
#ifndef INC_MAIN_H_
#define INC_MAIN_H_

/************************** HEADER FILES INCLUDES *****************************/

/* Used to import all necessary header files */
#include <application.h>

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
 *	\brief	:	Contains the IDs of all system states.
 *	\see	:	main and application functions.
 **/
typedef enum
{
	FIRST_RUN			,
	NEW_PASSWORD		,
	CONFIRM_PASSWORD	,
	MATCHED_PASSWORD	,
	MISMACHTED_PASSWORD	,
	ENTER_PASSWORD		,
	MAIN_MENU			,
	DOOR_OPEN			,
	DOOR_HOLD			,
	DOOR_CLOSE			,
	WRONG_PASSWORD		,
	LOCK_SYSTEM
} systemStateType;

/************************ GLOBAL VARIABLES EXTERN *****************************/

extern systemStateType g_current_control_state;

#endif /* INC_MAIN_H_ */
