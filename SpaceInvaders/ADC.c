#include "ADC.h"
#include "..//tm4c123gh6pm.h"

void ADC_Init(void){
	unsigned long delay;
	SYSCTL_RCGC2_R			|=	0x10;
	SYSCTL_RCGCGPIO_R  	|= 	0x10;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_DEN_R 		&= 	~0x4;
	GPIO_PORTE_DIR_R 		&=	~0x4;
	GPIO_PORTE_AMSEL_R 	|= 	0x4;
	GPIO_PORTE_AFSEL_R 	|= 	0x4;
	SYSCTL_RCGCADC_R 		|=	0x01;
	delay= SYSCTL_RCGCADC_R;
	SYSCTL_RCGC0_R 			|= 	0x10000;
	delay = SYSCTL_RCGC0_R;
	ADC0_SSPRI_R 				= 	0x0123;
	ADC0_ACTSS_R				&= 	~0x08;
	ADC0_EMUX_R					&= 	~0xF000;
	ADC0_SSMUX3_R				= 	0x1;
	ADC0_SSCTL3_R 			= 	0x6;
	ADC0_ACTSS_R				|= 	0x08;
}
unsigned long ADC_In(void){
	unsigned long data;
	ADC0_PSSI_R |= 0x8;
	while ((ADC0_RIS_R & 0x8) == 0){};
	data = (ADC0_SSFIFO3_R & 0xFFF);
	ADC0_ISC_R 	|= 0x8;
	return data;
}
