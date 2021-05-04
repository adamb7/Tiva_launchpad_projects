// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************Counter for Systick************
unsigned long count = 0;
// **************Sinewave data for DAC**********
const unsigned long Sinewave_Data[] = {0x1,0x1,0x2,0x2,0x3,0x4,0x5,0x7,0x8,0x9,0xB,0xC,0xD,0xE,0xE,0xF,0xF,0xF,0xE,0xE,0xD,0xC,0xB,0x9,0x8,0x7,0x5,0x4,0x3,0x2,0x2,0x1};

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
	NVIC_ST_CTRL_R 		= 0x00;
	NVIC_ST_RELOAD_R 	= 0x00;
	NVIC_ST_CURRENT_R	= 0x00;
	NVIC_ST_CTRL_R 		= 0x07;
	DAC_Init();
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	NVIC_ST_RELOAD_R = 	period-1;
	DAC_Out(Sinewave_Data[count]);
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	NVIC_ST_RELOAD_R 	= 0x00;
	DAC_Out(0x0);
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	count = (count & 0x1F) + 1;
}
