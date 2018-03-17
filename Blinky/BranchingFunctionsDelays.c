// BranchingFunctionsDelays.c Lab 6
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to 
// toggle an LED while a button is pressed and 
// turn the LED on when the button is released.  
// This lab will use the hardware already built into the LaunchPad.
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// built-in connection: PF0 connected to negative logic momentary switch, SW2
// built-in connection: PF1 connected to red LED
// built-in connection: PF2 connected to blue LED
// built-in connection: PF3 connected to green LED
// built-in connection: PF4 connected to negative logic momentary switch, SW1

#include "TExaS.h"

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Init_PortF(void); //Init port F
unsigned long data_in;
unsigned long data_out;

int main(void){ unsigned long volatile delay;
  TExaS_Init(SW_PIN_PF4, LED_PIN_PF2);  // activate grader and set system clock to 80 MHz
  // initialization goes here
	Init_PortF();
	

  EnableInterrupts();           // enable interrupts for the grader
  while(1){
    // body goes here
		data_in = ~GPIO_PORTF_DATA_R;
		data_in &= 0x10;
		data_in = data_in >>2;
		data_out = GPIO_PORTF_DATA_R;
		data_out &= 0xFB;
		data_out |= data_in;
		GPIO_PORTF_DATA_R = data_out;

		
  }
}


void Init_PortF(void){
	volatile unsigned long delay;
	//page 657	
	//set RCGCGPIO register clock
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; 
	//delay
	delay = SYSCTL_RCGC2_R;
	//GPIODIR
	GPIO_PORTF_DIR_R = 0x0E;
	//GPIOAFSEL & GPIOPCTL
	GPIO_PORTF_AFSEL_R  = 0x00;
	GPIO_PORTF_PCTL_R		= 0x00;
	//GPIOPUR
	GPIO_PORTF_PUR_R = 0x11;
	//GPIODEN
	GPIO_PORTF_DEN_R = 0x1F;
	//GPIOAMSEL
	GPIO_PORTF_AMSEL_R = 0x00;
	
}
