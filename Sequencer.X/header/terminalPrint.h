/* 
 * File:   terminalPrint.h
 * Author: austin
 *
 * Created on May 31, 2019, 11:06 AM
 *	
 * Simple Library to send bytes to the terminal of a computer
 * 
 * @VERSION: 1.2
 * 
 * @NOTE: To set the baud rate change the BAUDRATE definition
 *
 */

//user defined baudrate
#define BAUDRATE 115200

#ifndef TERMINALPRINT_H
#define	TERMINALPRINT_H

#include <string.h>
#include <avr/io.h>
#include <stdlib.h>

#define USART3_BAUD_RATE(BAUD_RATE) ((float)(3333333.33333 * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void USART3_init();
void USART3_sendChar(char c);
void USART3_sendString(char *str);
void USART3_sendByte(uint8_t);
void USART3_sendNum(uint8_t);
void USART3_sendHex(uint8_t);
uint8_t USART3_read();

#endif	/* TERMINALPRINT_H */

