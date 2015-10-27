#include "MKL25Z4.h"
#include "pwm.h"
#include "led.h"



void TPM1_IRQHandler(void) {
	//clear pending IRQ
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	
	
	// clear the overflow mask by writing 1 to TOF
	
	if (TPM1->SC & TPM_SC_TOF_MASK) {
		TPM1->SC |= TPM_SC_TOF_MASK;	
		LEDRed_On();
		LEDGreen_On();
		LEDBlue_On();
	}
	if(TPM1->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK) {
		TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
		LEDRed_Off();
	}
	if(TPM1->CONTROLS[1].CnSC & TPM_CnSC_CHF_MASK) {
		TPM1->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;
		LEDGreen_Off();
	}
	
}

/*

function name: initPWM

Freq. = (48 MHz / 2^prescaleShift) / period

parameters: TPMx, channel, centerAligned, 

module: choose the TPMx module
channel: channel select
centerAligned: center aligned PWM if 1, else edge aligned
prescaleShift: divide counter by 2^n
lowTrue: output high on channel match, else output low
TOIE: overflow interrupt enable
CHIE: channel interrupt enable
period
pulseWidth

*/

void initPWM(int module, int channel, int centerAligned, int prescaleShift, int lowTrue, int TOIE, int CHIE, uint32_t period, uint32_t pulseWidth) {
	
	TPM_Type* TPMx;	
	
// Set up the clock source for MCGPLLCLK/2. 
// See p. 124 and 195-196 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
// TPM clock will be 48.0 MHz if CLOCK_SETUP is 1 in system_MKL25Z4.c.
	
	SIM-> SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
	
	if(pulseWidth > period)
		return;
	
	// See p. 207 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	// Turn on clock to TPMx
	switch(module) {
		case 0 : 
			SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; 
		break;
		case 1:
			SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
		break;
		case 2:
			SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
		break;
	}

	// set base pointer
	switch(module) {
		case 0 : 
			TPMx = TPM0;
		break;
		case 1:
			TPMx = TPM1;
		break;
		case 2:
			TPMx = TPM2;
		break;
	}
	
	// See p. 163 and p. 183-184 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	//PORTB->PCR[1] = PORT_PCR_MUX(3); // Configure PTB1 as TPM1_CH1
	
	// See p. 555 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	TPMx->CONTROLS[channel].CnSC = 0;
	// Set channel TPMx_CHx as PWM
	TPMx->CONTROLS[channel].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	
	// enable interrupt from channel
	if(CHIE)
		TPMx->CONTROLS[channel].CnSC |= TPM_CnSC_CHIE_MASK;

	// set polarity
	if(lowTrue)
		TPMx->CONTROLS[channel].CnSC |= TPM_CnSC_ELSA_MASK;

	// See p. 552 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	TPMx->SC = 0;
	// set channel TPMx_CHx type
	if(centerAligned)
		TPMx->SC |= TPM_SC_CPWMS_MASK;
	else
		TPMx->SC &= ~TPM_SC_CPWMS_MASK;

	// set TPMx to up-counter, divide by prescaler and clock mode
	TPMx->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(prescaleShift);
	
	// enable interrupt from TOF
	if(TOIE)
		TPMx->SC |= TPM_SC_TOIE_MASK;
	
	// clear the overflow mask by writing 1 to TOF
	
	if (TPMx->SC & TPM_SC_TOF_MASK) 
		TPMx->SC |= TPM_SC_TOF_MASK;
	
	// Set period and pulse widths
	// See p. 567 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	TPMx->MOD = period-1;		// Freq. = (48 MHz / 2^prescaleShift) / period
	TPMx->CONTROLS[channel].CnV = pulseWidth; 	
	

	// Enable Interrupts
	if(TOIE || CHIE) {
		switch(module) {
			case 0:
				NVIC_SetPriority(TPM0_IRQn, 192); // 0, 64, 128 or 192
				NVIC_ClearPendingIRQ(TPM0_IRQn); 
				NVIC_EnableIRQ(TPM0_IRQn);	
			break;
			case 1:
				NVIC_SetPriority(TPM1_IRQn, 192); // 0, 64, 128 or 192
				NVIC_ClearPendingIRQ(TPM1_IRQn); 
				NVIC_EnableIRQ(TPM1_IRQn);				
			break;
			case 2:				
				NVIC_SetPriority(TPM2_IRQn, 192); // 0, 64, 128 or 192
				NVIC_ClearPendingIRQ(TPM2_IRQn); 
				NVIC_EnableIRQ(TPM2_IRQn);	
			break;
		}
	}
}


void setPeriod(int module, uint32_t countTo) {
	TPM_Type* TPMx;	

	// set base pointer
	switch(module) {
		case 0 : 
			TPMx = TPM0;
		break;
		case 1:
			TPMx = TPM1;
		break;
		case 2:
			TPMx = TPM2;
		break;
	}
	TPMx->MOD = countTo-1;
}


void setPulseWidth(int module, int channel, uint32_t pulseWidth) {
	TPM_Type* TPMx;	

	// set base pointer
	switch(module) {
		case 0 : 
			TPMx = TPM0;
		break;
		case 1:
			TPMx = TPM1;
		break;
		case 2:
			TPMx = TPM2;
		break;
	}
	TPMx->CONTROLS[channel].CnV = pulseWidth; 	
}


// setTone assumes prescaler 7, so TPM clk = 375 khz
// 100 < hz < 10000
void setTone(int module, int channel, int freq) {
	int countTo;
	
	if(freq > 15 && freq < 3000)
		countTo = 375000 / freq;
	else
		countTo = 375000 / 440; // 440 hz is default
	
	setPeriod(module, countTo);
	setPulseWidth(module, channel, countTo / 2);
}


void enablePWM(int module) {
TPM_Type* TPMx;	

	// set base pointer
	switch(module) {
		case 0 : 
			TPMx = TPM0;
		break;
		case 1:
			TPMx = TPM1;
		break;
		case 2:
			TPMx = TPM2;
		break;
	}
	TPMx->SC |= TPM_SC_CMOD(1);
}


void disablePWM(int module) {
TPM_Type* TPMx;	

	// set base pointer
	switch(module) {
		case 0 : 
			TPMx = TPM0;
		break;
		case 1:
			TPMx = TPM1;
		break;
		case 2:
			TPMx = TPM2;
		break;
	}
	TPMx->SC &= TPM_SC_CMOD(0);
}
