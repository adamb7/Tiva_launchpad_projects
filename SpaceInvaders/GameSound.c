// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

#include "GameSound.h"
#include "..//tm4c123gh6pm.h"

#define ARRAY_SIZE 32

const unsigned long Sinewave_Data[] = {0x1,0x1,0x2,0x2,0x3,0x4,0x5,0x7,0x8,0x9,0xB,0xC,0xD,0xE,0xE,0xF,0xF,0xF,0xE,0xE,0xD,0xC,0xB,0x9,0x8,0x7,0x5,0x4,0x3,0x2,0x2,0x1};
unsigned long TimerCount;
//unsigned long Semaphore;
unsigned long SampleCount = 0;

void DACInit(){
	unsigned long delay;
	SYSCTL_RCGC2_R		|= 0x3;
	SYSCTL_RCGCGPIO_R |= 0x3;
	delay=SYSCTL_RCGCGPIO_R;
	GPIO_PORTB_DEN_R	|=0x3F;
	GPIO_PORTB_AFSEL_R &=0x00;
	//GPIO_PORTB_AMSEL_R&=~0x3F;
	GPIO_PORTB_DIR_R	|=0x3F;
	//GPIO_PORTB_PCTL_R	&=	~0xFFFF;//set pins as gpio
	GPIO_PORTB_DR8R_R |= 	0xF;
}
void DACOut(unsigned long counter){
	GPIO_PORTB_DATA_R = (Sinewave_Data[counter]&0xF);
}
void SetTone(unsigned long sound){
	//for different sounds
	TIMER2_CTL_R = 0x00000000;
	TIMER2_TAILR_R = sound-1; //set reload reg
	TIMER2_CTL_R = 0x00000001;
}
void GameSound_Play(unsigned long sound){
	//SetTone(sound);
	//SampleCount = ARRAY_SIZE*8;
	SampleCount = 11000;
}
// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  //Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void Timer2A_Handler(void){
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  TimerCount++;
  //Semaphore = 1; // trigger
	if(SampleCount>0){
		DACOut(TimerCount&0x1F); //samples[0.....31]
		SampleCount--;
	}
	else{
		DACOut(0);
	}
}
void GameSound_Init(){
	DACInit();
	Timer2_Init(227); //11kHz
}
