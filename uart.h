/*
 * File:		uart.h
 * Purpose:     Provide common ColdFire uart routines for polled serial IO
 *
 * Notes:
 */

#ifndef __uart_H__
#define __uart_H__

#include <MKL25Z4.h>
/********************************************************************/


void uart0_init (int sysclk, int baud);
void initUart(void);
char uart0_getchar (void);
void uart0_putchar (char ch);
void uart0_put(char* str);
void uart0_printNumber(unsigned long n, uint8_t base, int negative);
int uart0_getchar_present (void);


/********************************************************************/

#endif /* __uart_H__ */
