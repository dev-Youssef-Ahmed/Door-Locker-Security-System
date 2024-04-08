 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Mohamed Tarek - \note: modified by Youssef Ahmed
 *
 *******************************************************************************/

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */

/* Holds the address of the callback function */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/* ISR that will execute when "USART, Rx Complete" interrupt occurs */
ISR(USART_RXC_vect)
{
	/*
	 *  A USART Receive Complete Interrupt will be generated only if the RXCIE
	 *  bit is written to one, the Global Interrupt Flag in SREG is written to
	 *  one and the RXC bit in UCSRA is set.
	 */

	/* Checks if the global pointer g_callBackPtr is not pointing to NULL. */
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the callback function using pointer to function g_callBackPtr(); */
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void UARTRX_setCallBack(void(*a_ptr)(void))
{

	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}


/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_Init(const UART_ConfigType *Config_Ptr)
{
#ifdef	U2X_MODE
	/* U2X = 1 for double transmission speed */
    UCSRA |= (1 << U2X);
#endif

    /* Enable receiver, transmitter, and RX complete interrupt */
    UCSRB = (ENABLE << TXEN) | (ENABLE << RXEN) | (ENABLE << RXCIE);

    /* Set frame format: data bits, parity, stop bits */
    uint8 UCSRC_value = 0;
    UCSRC_value = (ENABLE << URSEL);

    switch (Config_Ptr->bit_data)
    {
        case DATA_5_BIT	:
        {
        	break;
        }
        case DATA_6_BIT	:
        {
            UCSRC_value |= (ENABLE << UCSZ0);
            break;
        }
        case DATA_7_BIT	:
        {
        	UCSRC_value |= (ENABLE << UCSZ1);
            break;
        }

        case DATA_8_BIT	:
        {
            UCSRC_value |= (ENABLE << UCSZ0) | (ENABLE << UCSZ1);
            break;
        }

        case DATA_9_BIT	:
        {
            UCSRC_value |= (ENABLE << UCSZ0) | (ENABLE << UCSZ1) | (ENABLE << UCSZ2);
            break;
        }
    }

    switch (Config_Ptr->parity)
    {
		case NO_PARITY	:
		{
			break;
		}
        case EVEN_PARITY:
        {
           	UCSRC_value |= (ENABLE << UPM1);
           	break;
        }
        case ODD_PARITY	:
        {
        	UCSRC_value |= (ENABLE << UPM1) | (ENABLE << UPM0);
        	break;
        }
    }

    switch (Config_Ptr->stop_bit)
    {
        case STOP_BIT_1	:
        {
            break;
        }
        case STOP_BIT_2	:
        {
            UCSRC_value |= (ENABLE << USBS);
            break;
        }
    }

    UCSRC = UCSRC_value;
	
	/* Calculate the UBRR register value */
	 uint16 ubrr_value = (uint16_t)(((F_CPU / ((Config_Ptr->baud_rate) * BITS_PER_UART_FRAME))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = (uint8)(ubrr_value>>8);
	UBRRL = (uint8)ubrr_value;
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;

	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
    return UDR;		
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	*******************************************************************/
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_recieveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}

/*
 * Description :
 * Functional responsible for send multiple bytes to another UART device.
 */
void UART_sendGolabalFrame(const uint8_t *frame_TX, uint8_t number_of_bytes)
{
	uint8 byte_index;
	// Iterate through each byte in the data array
	for (byte_index = 0 ; byte_index < number_of_bytes ; byte_index++)
	{
		// Send the current byte
		UART_sendByte(frame_TX[byte_index]);
	}
}

/*
 * Description :
 * Functional responsible for receive multiple bytes from another UART device.
 */
void UART_receiveGlobalFrame(uint8_t *frame_RX, uint8_t number_of_bytes)
{
	uint8 byte_index;
	// Iterate through each byte to be received
	for (byte_index = 0 ; byte_index < number_of_bytes ; byte_index++)
	{
		// Receive the byte and store it in the buffer
		frame_RX[byte_index] = UART_recieveByte();
	}
}
