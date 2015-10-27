/*----------------------------------------------------------------------------

Soundball project for my stay in Tanzania

Changes speaker tone depending on change in acceleration

Made by Andrew Yunseok Chung

 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>
#include "led.h"
#include "i2c.h"
#include "mma8451.h"
#include "pwm.h"
#include "delay.h"
#include "uart.h"

#define BUFFER_SIZE 20

void printAccel(int16_t value) {
	// check 14th bit
	//if(value & 0x2000) {
	if(value < 0) {
		value *= -1;
		uart0_printNumber(value, 10, 1);
	}
	else
		uart0_printNumber(value, 10, 0);
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	int freq;
	int mode;
	char key;
	int skip, changeSound;
	int prevMax;
	int16_t prev_acc_X, prev_acc_Y, prev_acc_Z;
	int16_t avgAccX, avgAccY, avgAccZ;
	int16_t accXArray[BUFFER_SIZE], accYArray[BUFFER_SIZE], accZArray[BUFFER_SIZE];
	int i;
	int notes[15] = { 220, 247, 277, 294, 330, 370, 415, 440, 494, 554, 587, 659, 740, 784, 880 };
	
	key = 0;
	LED_Initialize();
	initUart();
	initPWM(1,1,0,7,0,0,0,700,350);
	PORTB->PCR[1] |= PORT_PCR_MUX(3); // Configure PTB1 as TPM1_CH1
	PORTB->PCR[1] |= PORT_PCR_DSE_MASK;
	
	i2c_init();									/* init i2c	*/
	if (!init_mma()) {							/* init mma peripheral */
		LEDRed_On();							/* Light red error LED */
		while (1);								/* not able to initialize mma */
	}

	/*** initiailize xyz avgerages ***/
	prev_acc_X = prev_acc_Y = prev_acc_Z = 0;
	avgAccX = avgAccY = avgAccZ = 0;
	for(i = 0; i < BUFFER_SIZE; i++) {
		Delay(1);
		read_full_xyz();
		accXArray[i] = acc_X;
		accYArray[i] = acc_Y;
		accZArray[i] = acc_Z;
		
		avgAccX += acc_X / BUFFER_SIZE;
		avgAccY += acc_Y / BUFFER_SIZE;
		avgAccZ += acc_Z / BUFFER_SIZE;
	}
	
	
	uart0_put("\r\nStart\r\n");
	
	mode = 1; // mode is set here
	skip = 0;
	changeSound = 0;

	while (1) {
		
		if(uart0_getchar_present()) {
			key = uart0_getchar();
			
			if(key == 'p') {
				disablePWM(1);
				uart0_put("\r\n************** Paused *************\r\n");
				
				while(key != 'c') {
					key = uart0_getchar();
					
					if(key == '0') {
						uart0_put("Mode = 0\r\n");
						mode = 0;
					}
					else if(key == '1') {
						uart0_put("Mode = 1\r\n");
						mode = 1;	
					}
				}
				
				enablePWM(1);
				uart0_put("************** Resume *************\r\n\r\n");
			}
		}
		
		//read_full_xyz();
		
		if(mode == 0) {
			int threshold = 400;
			int16_t max;
			
			Delay(100);
			if(skip > 0) {
				uart0_put("Skip\t\t\t\t   ");
				skip--;
			}
			else {
				max = acc_X - prev_acc_X;
				if(fabs(acc_Y - prev_acc_Y) > fabs(max))
					max = acc_Y - prev_acc_Y;
				if(fabs(acc_Z - prev_acc_Z) > fabs(max))
					max = acc_Z - prev_acc_Z;
				
				if(fabs(max) > threshold) {
					if(max > 0)
						freq = 880;
					else freq = 220;
					skip = 2;				// skip is set here
				}
				else freq = 440;
				
				uart0_put("Max: ");
				printAccel(max);
				uart0_put("\tFreq: ");
				uart0_printNumber(freq, 10, 0);
				uart0_putchar('\t');
				
				setTone(1,1,freq);
			}
		}
		else if(mode == 1) {
			int initialThreshold = 1500;
			int16_t max;
			
			
			if(skip > 0) {
				uart0_put("Skip                         ");
				skip--;
			}
			else {
				max = avgAccX - prev_acc_X;
				if(fabs(avgAccY - prev_acc_Y) > fabs(max))
					max = avgAccY - prev_acc_Y;
				if(fabs(avgAccZ - prev_acc_Z) > fabs(max))
					max = avgAccZ - prev_acc_Z;
								
				if(changeSound) {
					if((max > 0 && prevMax > 0 ) || (max < 0 && prevMax < 0)) { // direction is the same
						if(max > 0) {
							int note = max / 500 + 7;
							if(note >= 15)
								note = 15;
							freq = notes[note];
							
							LEDOff();
							LEDRed_On();
							uart0_put("     note: ");
							uart0_printNumber(note, 10, 0);
							uart0_put("        ");
							
						}
						else {
							int note = 7 - fabs(max) / 500;
							if(note < 0)
								note = 0;
							freq = notes[note];
							LEDOff();
							LEDGreen_On();
							uart0_put("     note: ");
							uart0_printNumber(note, 10, 0);
							uart0_put("        ");
						}
					}
					else { // direction not the same
						skip = 10;
						changeSound = 0;
					}
				}
				else {
					if(fabs(max) > initialThreshold) { // not changing sound and past threshold
						if(max > 0) {
							int note = max / 500 + 7;
							if(note >= 15)
								note = 15;
							freq = notes[note];
							
							LEDOff();
							LEDRed_On();
							uart0_put("     note: ");
							uart0_printNumber(note, 10, 0);
							uart0_put("        ");
						}
						else {
							int note = 7 - fabs(max) / 500;
							if(note < 0)
								note = 0;
							freq = notes[note];
							LEDOff();
							LEDGreen_On();
							uart0_put("     note: ");
							uart0_printNumber(note, 10, 0);
							uart0_put("        ");
						}
						
						changeSound = 1;
					}
					else {
						freq = 440;
						LEDOff();
						LEDBlue_On();
					}
				}				
				
				prevMax = max;

				uart0_put("Max: ");
				printAccel(max);
				uart0_put("     Freq: ");
				uart0_printNumber(freq, 10, 0);
				uart0_put("        ");
				
				setTone(1,1,freq);
			}
		}
		
		prev_acc_X = avgAccX;
		prev_acc_Y = avgAccY;
		prev_acc_Z = avgAccZ;
		
		
		uart0_put("X: ");
		printAccel(avgAccX);
		uart0_put("        ");
		uart0_put("Y: ");
		printAccel(avgAccY);
		uart0_put("        ");
		uart0_put("Z: ");
		printAccel(avgAccZ);
		
		/*
		convert_xyz_to_roll_pitch();
		uart0_put("        ");
		uart0_put("r: ");
		printAccel(roll);
		uart0_put("        ");
		uart0_put("p: ");
		printAccel(pitch);
		*/
		
		for(i = 0; i < BUFFER_SIZE; i++) {
			Delay(1);
			read_full_xyz();
			
			avgAccX -= accXArray[i] / BUFFER_SIZE;
			accXArray[i] = acc_X;
			avgAccX += acc_X / BUFFER_SIZE;
			
			avgAccY -= accYArray[i] / BUFFER_SIZE;
			accYArray[i] = acc_Y;
			avgAccY += acc_Y / BUFFER_SIZE;
			
			avgAccZ -= accZArray[i] / BUFFER_SIZE;
			accZArray[i] = acc_Z;
			avgAccZ += acc_Z / BUFFER_SIZE;
		}
		
		uart0_put("\r\n");
		
	}
}

