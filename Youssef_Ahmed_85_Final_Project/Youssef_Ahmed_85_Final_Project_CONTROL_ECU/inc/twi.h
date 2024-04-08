/********************************************************************************
 *																				*
 *		\brief:	Two-wire Serial Interface Driver	-	Header file				*
 *																				*
********************************************************************************/

/*
 * 	Header Guard:	It's a preprocessor directive used to prevent a header file
 * 	from being included multiple times in the same compilation unit, which could
 * 	lead to issues like redefinition errors.
 */
#ifndef TWI_H_
#define TWI_H_

#include <avr/io.h>			/* Contains AVR registers and bits macros */
#include <common_macros.h>	/* Contains bit manipulation macros */
#include <std_types.h>		/* Contains the standard data types */
//#include <my_defs.h>		/* Contains all other macros */

/********************************************************************************
 *					\note:	Section #1:	Macros of TWI							*
********************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define TWI_START			0x08 /* start has been sent */
#define TWI_REP_START		0x10 /* repeated start */
#define TWI_MT_SLA_W_ACK	0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define TWI_MT_SLA_R_ACK	0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define TWI_MT_DATA_ACK		0x28 /* Master transmit data and ACK has been received from Slave. */
#define TWI_MR_DATA_ACK		0x50 /* Master received data and send ACK to slave. */
#define TWI_MR_DATA_NACK	0x58 /* Master received data but doesn't send ACK to slave. */

/* Each macro is meaningful as its name */
#define TWI_BIT_RATE_PRESCALER	0x00
#define TWSR_STATUS_BITS		0xF8
#define TWAR_NON_GCR_BITS		0xFE

/********************************************************************************
 *					\note:	Section #2:	Configurations of TWI					*
********************************************************************************/



typedef uint8 TWI_Address;
typedef uint32 TWI_BaudRate;

typedef struct
{
	TWI_Address		address;
	TWI_BaudRate	bit_rate;
} TWI_ConfigType;

/********************************************************************************
 *			\note:	Section #3:	TWI Driver Functions Prototypes					*
********************************************************************************/

/**
 *	\brief	:	Initiates the configurable TWI driver.
 *	\param	:	Configurations structure of "\see: TWI_ConfigType".
 *	\return	:	Void
 **/
void TWI_init(const TWI_ConfigType * Config_Ptr);


void TWI_start(void);


void TWI_stop(void);


void TWI_writeByte(uint8 data);


uint8 TWI_readByteWithACK(void);


uint8 TWI_readByteWithNACK(void);


uint8 TWI_getStatus(void);


#endif /* TWI_H_ */
