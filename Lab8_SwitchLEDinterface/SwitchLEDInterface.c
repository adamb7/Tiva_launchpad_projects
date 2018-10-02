// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
#define PORT_E 			(*((volatile unsigned long*) 0x40024000))
#define RCGC2GPIO 		(*((volatile unsigned long*) 0x400FE108))
#define GPIO_DIR 		(*((volatile unsigned long*) 0x40024400))
#define GPIO_AFSEL	(*((volatile unsigned long*) 0x40024420))
#define GPIO_DR2R		(*((volatile unsigned long*) 0x40024500))
#define GPIO_DEN		(*((volatile unsigned long*) 0x4002451C))
#define GPIO_DATA		(*((volatile unsigned long*) 0x400243FC))

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void INIT_E_PORT(void);
void Delay100ms(unsigned long time);



// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  INIT_E_PORT();
	//GPIO_DATA |= 0x02;
	GPIO_DATA |= 1UL << 1;
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
		//GPIO_DATA |= 0x02;
		//wait 100ms
		Delay100ms(1);
		//if switch pressed => toggle
		if(GPIO_DATA & 0x01){
			//toggle
			GPIO_DATA ^= 1UL << 1;
		}
		else{
			GPIO_DATA |= 0x02;
		}
  }
  
}

void INIT_E_PORT(){
	volatile unsigned long delay;
	//clock
	RCGC2GPIO |= 0x00000010;
	//delay for clock
	delay = RCGC2GPIO;
	//gpiodir
	GPIO_DIR |= 0x02; 
	//gpioafsel
	GPIO_AFSEL |= 0x00;
	//gpio2dr
	GPIO_DR2R |= 0x02;
	//gpioden
	GPIO_DEN |= 0x03;
}
void Delay100ms(unsigned long time){
	unsigned long i;
	while(time >0){
		i = 1333333; //The number 1333333 assumes 6 cycles per loop (100ms/12.5ns/6)
		while(i>0){
			i -=1;
		}
		time -=1;
	}
}
