#include "ADC.h"
#include "..//tm4c123gh6pm.h"

// Slide pot pin 2 connected to PE2/AIN1
// Sample sequencer 3 is being used
unsigned long delay;
void ADC_Init(void){
	SYSCTL_RCGC2_R			|=	0x10;
	SYSCTL_RCGCGPIO_R  	|= 	0x10;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_DEN_R 		&= 	~0x4;
	GPIO_PORTE_DIR_R 		&=	~0x4;
	GPIO_PORTE_AMSEL_R 	|= 	0x4;
	GPIO_PORTE_AFSEL_R 	|= 	0x4;
	SYSCTL_RCGCADC_R 		|=	0x01;
	delay= SYSCTL_RCGCADC_R;
	SYSCTL_RCGC0_R 			|= 	0x10000;		//ss3 clock
	delay = SYSCTL_RCGC0_R;
	ADC0_SSPRI_R 				= 	0x0123; //ss3 top prio
	ADC0_ACTSS_R				&= 	~0x08;	//stop
	ADC0_EMUX_R					&= 	~0xF000; //processor trigger will start SS3
	ADC0_SSMUX3_R				= 	0x1;	//sample comes from Ain1
	ADC0_SSCTL3_R 			= 	0x6;	//enable interrupt generation(assert INR0 bit at the end of this sample conversion), and end of sequence bit
	//ADC0_IM_R 					=		0x00000008;	//set IR mask
	ADC0_ACTSS_R				|= 	0x08;	//start SS3
}
unsigned long ADC_In(void){
	unsigned long data;
	ADC0_PSSI_R |= 0x8;
	while((ADC0_RIS_R & 0x8)==0){};
	data = (ADC0_SSFIFO3_R & 0xFFF);
	ADC0_ISC_R 	|= 0x8;
	return data;
}
