#include "Switch.h"
#include "..//tm4c123gh6pm.h"

void Switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x12; //init PortE 
	GPIO_PORTE_DEN_R 	|=0x3;	//input: PE0,PE1
	GPIO_PORTE_AFSEL_R&=~0x3; // fire button connected to PE0,special weapon fire button connected to PE1
	GPIO_PORTE_AMSEL_R&=~0x3;
	GPIO_PORTE_DIR_R	&=~0x3;
}

unsigned char Switch_Read(void){
	return (GPIO_PORTE_DATA_R & 0x3);
}
