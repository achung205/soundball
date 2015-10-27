#include "MKL25Z4.h"
#include "freedom.h"

void initFreedom(void) {
	
	
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK
								| SIM_SCGC5_PORTB_MASK
								| SIM_SCGC5_PORTC_MASK
								| SIM_SCGC5_PORTD_MASK
								| SIM_SCGC5_PORTE_MASK );
	
}

