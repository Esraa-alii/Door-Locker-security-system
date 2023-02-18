 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Esraa Ali
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                         Definitions                                   *
 *******************************************************************************/
#define USART_BAUDRATE 9600

/*******************************************************************************
 *                      Type Declaration                                   *
 *******************************************************************************/
typedef enum{
	Async,sync
}UART_mode;

typedef enum{
	disabled,reserved,enable_evenPolarity,enable_oddPolarity
}UART_polarity_mode;

typedef enum{
	one_bit,two_bit
}UART_stopBit;

typedef struct{
	UART_mode mode;
	UART_polarity_mode polarity_mode;
	UART_stopBit stopBit;
	uint32 baud_rate;
}UART_config;

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
void UART_init(const UART_config *configuration);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

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

#endif /* UART_H_ */
