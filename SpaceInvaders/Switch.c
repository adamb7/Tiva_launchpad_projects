#include "Switch.h"
#include "..//tm4c123gh6pm.h"

extern void Delay100ms(unsigned long count);
extern void Random_Init(unsigned long seed);
extern unsigned char pause;

unsigned char Switch_Read(void){
	return (GPIO_PORTE_DATA_R & 0x2);
}

void InterruptInit(void){
	//interrupt for PE0
	GPIO_PORTE_IM_R = 0x00;
	NVIC_EN0_R |= 0x00000010;
	GPIO_PORTE_IS_R = 0x00;
	GPIO_PORTE_IBE_R = 0x00;
	GPIO_PORTE_IEV_R = 0x01;
	GPIO_PORTE_IM_R = 0x01;
}

void GPIOPortE_Handler(void){
	GPIO_PORTE_IM_R = 0x00;
	pause ^= 1U;
	Delay100ms(2);
	Random_Init(NVIC_ST_CURRENT_R);
	GPIO_PORTE_ICR_R = 0x1;
	GPIO_PORTE_IM_R = 0x01;
}
void Switch_Init(void){
	//input: PE0,PE1
	SYSCTL_RCGCGPIO_R |= 0x12;
	GPIO_PORTE_DEN_R 	|= 0x3;	
	GPIO_PORTE_AFSEL_R &= ~0x3; 
	GPIO_PORTE_AMSEL_R &= ~0x3;
	GPIO_PORTE_DIR_R	&= ~0x3;
	GPIO_PORTE_PDR_R	|= 0x3;
	InterruptInit();
}
