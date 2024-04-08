 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"
#include <avr/interrupt.h>

#define U2X_MODE
#define BITS_PER_UART_FRAME	(8UL)

typedef enum
{
    DATA_5_BIT	,
    DATA_6_BIT	,
    DATA_7_BIT	,
    DATA_8_BIT	,
	DATA_9_BIT = 7
} UART_BitDataType;

typedef enum
{
    NO_PARITY	,
    EVEN_PARITY = 2	,
    ODD_PARITY
} UART_ParityType;

typedef enum
{
    STOP_BIT_1	,
    STOP_BIT_2
} UART_StopBitType;

typedef uint32 UART_BaudRateType;

// Define UART configuration structure
typedef struct
{
    UART_BitDataType	bit_data	;
    UART_ParityType		parity		;
    UART_StopBitType	stop_bit	;
    UART_BaudRateType	baud_rate	;
} UART_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_Init(const UART_ConfigType *Config_Ptr);
/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);
/*
 * Description :
 * Functional responsible for send multiple bytes to another UART device.
 */
void UART_sendGolabalFrame(const uint8_t *frame_TX, uint8_t number_of_bytes);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Functional responsible for receive multiple bytes from another UART device.
 */
void UART_receiveGlobalFrame(uint8_t *frame_RX, uint8_t number_of_bytes);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

void UARTRX_setCallBack(void(*a_ptr)(void));

#endif /* UART_H_ */
