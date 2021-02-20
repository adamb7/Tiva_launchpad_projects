// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result. 
// Daniel Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "ADC.h"
#include "..//tm4c123gh6pm.h"

// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void){ 
	unsigned long delay;
  GRADER_CLOCK			|=0x10;
	SYSCTL_RCGCGPIO_R |=0x10; 	//PE3
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_DIR_R 	&=~0x08; 	//input
	GPIO_PORTE_AFSEL_R|=0x08;
	GPIO_PORTE_DEN_R 	&=~0x08;
	GPIO_PORTE_AMSEL_R|=0x08;
	SYSCTL_RCGCADC_R 	=0x01;		//turn on ADC clock
	delay = SYSCTL_RCGCADC_R;
	SYSCTL_RCGC0_R |= 0x10000;	//turn on ADC clock (legacy)
	delay = SYSCTL_RCGC0_R;
	ADC0_SSPRI_R 			=0x0123;	//set prio
	ADC0_ACTSS_R 			&=0x08; 	//disable sequencer 3
	ADC0_EMUX_R				&=~0xF000;//processor (software) trigger
	ADC0_SSMUX3_R 		=0x0; 		//sample ADC0
	ADC0_SSCTL3_R			=0x6;			//set 0x2 for END of sequence and 0x4 to activate interrupt trigger
	ADC0_ACTSS_R 			|=0x08;		//enable seq 3
}
//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void) {
    unsigned long result;
    ADC0_PSSI_R |= 0x08; 									// initiate SS3
    while ((ADC0_RIS_R & 0x08) == 0) {}		// wait until conversion is done:
    result      = ADC0_SSFIFO3_R & 0xFFF; // mask with 12-bit binary
    ADC0_ISC_R |= 0x08;                   // clear the flag to start capturing another sample
    return result;                        // return the value of result
}
