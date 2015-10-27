#include "MKL25Z4.h"
#include "dac.h"


void initDAC(int v)
{
    SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;

    PORTE->PCR[30] |= PORT_PCR_MUX(0); // Configure PTE30 as DAC0_OUT

    // vout = (1 + vin) / 4096 * 3.3
    DAC0->DAT->DATH = (v & 0x00000F00) >> 8;
    DAC0->DAT->DATL = v & 0x000000FF;
    DAC0->C0 = DAC_C0_DACEN_MASK ;
    DAC0->C2 = DAC_C2_DACBFRP_MASK ;

}


void enableDAC(void) {
    DAC0->C0 |= DAC_C0_DACEN_MASK ;
}

void disableDAC(void) {
    DAC0->C0 &= ~DAC_C0_DACEN_MASK ;
}


void setDAC(int v)
{
    DAC0->DAT->DATH = (v & 0x00000F00) >> 8;
    DAC0->DAT->DATL =  v & 0x000000FF;
}

