#include "led.h"
#include "MKL25Z4.h"

#define LED_NUM     3                   /* Number of user LEDs                */
const uint32_t led_mask[] = {1UL << 18, 1UL << 19, 1UL << 1};

#define LED_RED    0
#define LED_GREEN   1
#define LED_BLUE  2

/*----------------------------------------------------------------------------
  Function that initializes LEDs
 *----------------------------------------------------------------------------*/
void LED_Initialize(void) {

  SIM->SCGC5    |= (1UL <<  10) | (1UL <<  12);      /* Enable Clock to Port B & D */ 
  PORTB->PCR[18] = (1UL <<  8);                      /* Pin PTB18 is GPIO */
  PORTB->PCR[19] = (1UL <<  8);                      /* Pin PTB19 is GPIO */
  PORTD->PCR[1]  = (1UL <<  8);                      /* Pin PTD1  is GPIO */

  FPTB->PDOR = (led_mask[0] | 
               led_mask[1] );          /* switch Red/Green LED off  */
  FPTB->PDDR = (led_mask[0] | 
               led_mask[1] );          /* enable PTB18/19 as Output */

  FPTD->PDOR = led_mask[2];            /* switch Blue LED off  */
  FPTD->PDDR = led_mask[2];            /* enable PTD1 as Output */

}

/*----------------------------------------------------------------------------
  Function that turns on/off Red LED
 *----------------------------------------------------------------------------*/
void LEDRed_On (void) {
  FPTB->PCOR   = led_mask[LED_RED];    /* Red LED On*/
}

void LEDRed_Off(void) {
  FPTB->PSOR   = led_mask[LED_RED];    /* Red LED Off*/
}

void LEDRed_Toggle(void) {
  FPTB->PTOR   = led_mask[LED_RED];
}

/*----------------------------------------------------------------------------
  Function that turns on/off Green LED
 *----------------------------------------------------------------------------*/
void LEDGreen_On (void) {
  FPTB->PCOR   = led_mask[LED_GREEN];   /* Green LED On*/
}

void LEDGreen_Off(void) {
  FPTB->PSOR   = led_mask[LED_GREEN];   /* Green LED Off*/
}

void LEDGreen_Toggle(void) {
  FPTB->PTOR   = led_mask[LED_GREEN];
}

/*----------------------------------------------------------------------------
  Function that turns on/off Blue LED
 *----------------------------------------------------------------------------*/
void LEDBlue_On (void) {
  FPTD->PCOR   = led_mask[LED_BLUE];  /* Blue LED On*/
}

void LEDBlue_Off(void) {
  FPTD->PSOR   = led_mask[LED_BLUE];  /* Blue LED Off*/
}

void LEDBlue_Toggle(void) {
  FPTD->PTOR   = led_mask[LED_BLUE];
}

void LEDOff(void) {
	LEDRed_Off();
	LEDGreen_Off();
	LEDBlue_Off();
}
