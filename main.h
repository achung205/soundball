/*
 * main.h
 *
 *  Created on: May 16, 2013
 *      Author: B34443
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <MKL25Z4.h>
#include "uart.h"

#define BUFFER_SIZE 128
#define EDGE_ERROR_SIZE 15
#define SPEED_ERROR_SIZE 15
#define PARSE_DELAY 1
#define BLIND_THRESHOLD 30
// assert(PRASE_DELAY < ERROR_SIZE)

/* Uses UART0 for both Open SDA and TWR-SER Tower card */
#define TERM_PORT_NUM       0

#define TERMINAL_BAUD       115200
#undef  HW_FLOW_CONTROL

#define UART_MODE POLLING
#define POLLING				0
#define INTERRUPT			1

/*
 * Misc. Defines
 */
#ifdef	FALSE
#undef	FALSE
#endif
#define FALSE	(0)

#ifdef	TRUE
#undef	TRUE
#endif
#define	TRUE	(1)

#ifdef	NULL
#undef	NULL
#endif
#define NULL	(0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)

#endif /* MAIN_H_ */
