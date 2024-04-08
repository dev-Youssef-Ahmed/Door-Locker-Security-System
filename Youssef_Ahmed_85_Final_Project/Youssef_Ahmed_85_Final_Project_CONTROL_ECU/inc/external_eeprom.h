 /******************************************************************************
 *
 * Module: External EEPROM
 *
 * File Name: external_eeprom.h
 *
 * Description: Header file for the External EEPROM Memory
 *
 * Author: Mohamed Tarek - \note: modified by Youssef Ahmed
 *
 *******************************************************************************/


#ifndef EXTERNAL_EEPROM_H_
#define EXTERNAL_EEPROM_H_

#include "std_types.h"
#include "twi.h"
#include  <util/delay.h>

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define ERROR				0
#define SUCCESS				1

#define EEPROM_POST_DELAY	_delay_ms(10)	/* Delay for EEPROM post-operation */


#define EEPROM_PASS_STATE_ADDR	0x09		/* The address of password state in EEPROM */
#define EEPROM_PASS_BYTE1_ADDR	0x0A	/* The address of password's 1st byte in EEPROM */
#define EEPROM_M24C16_INIT_VAL	0XFF

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

uint8 EEPROM_writeByte(uint16 u16addr,uint8 u8data);
uint8 EEPROM_readByte(uint16 u16addr,uint8 *u8data);
 
#endif /* EXTERNAL_EEPROM_H_ */
