// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
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


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

#define SYSTICK_STCTRL			(*((volatile unsigned long*)0xE000E010))
#define SYSTICK_STRELOAD		(*((volatile unsigned long*)0xE000E014))
#define SYSTICK_STRECURRENT	(*((volatile unsigned long*)0xE000E018))

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

volatile char soundState = 0;
volatile char sw = 0;
volatile char prevSw = 0;

// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){ 
	//activate Port A
	SYSCTL_RCGCGPIO_R |= 	0x0C; 		//activate clock
	GPIO_PORTA_DIR_R	|=	0x04; 		//set dir
	GPIO_PORTA_AFSEL_R&=	0x00; 		//set gpio function
	GPIO_PORTA_PDR_R	|= 	0x08; 		//set pulldown resistor for input
	GPIO_PORTA_DEN_R 	|=	0x0C;			//set digital for input
	GPIO_PORTA_PCTL_R &=	~0xFF00;	//config pfa3 & pfa2 as gpio
	//activate systick
	SYSTICK_STCTRL 		&= ~0x0;			//stop for config
	SYSTICK_STRELOAD 	|= 0x1631C;		//set STRELOAD register (16Mhz & 880 HZ -> 18181 -> B551), 80MHZ / 880HZ: 90908-> 1631C
	SYSTICK_STRECURRENT&= 0x00;			//clear STCURRENT register (any value!)
	SYSTICK_STCTRL		|= 0x07;			//config STCTRL and start
}

// called at 880 Hz
void SysTick_Handler(void){
	sw = GPIO_PORTA_DATA_R & 0x08;
	if(sw && prevSw == 0){
		soundState ^= 1UL;
	}
	if(soundState == 1){
		GPIO_PORTA_DATA_R ^= 0x04;
	}
	else{
		GPIO_PORTA_DATA_R &= ~0x04;
	}
	prevSw = sw;
}

int main(void){// activate grader and set system clock to 80 MHz
	TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();
  EnableInterrupts();   // enable after all initialization are done
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
		
  }
}
