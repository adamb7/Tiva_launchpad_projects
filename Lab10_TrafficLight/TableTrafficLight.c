// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
#define goWest 0
#define hWest 1
#define goSth 2
#define hSth 3
#define Walk 4
#define noWalk 5
#define Fl_off1 5
#define offWalk 6
#define Fl_on2 7
#define Fl_off2 8
#define	rSth	9
#define	rWest	10

#define LED (*((volatile unsigned long *)0x400253FC))
#define FLASH_DELAY 800 //200 -> sim, 400 -> real
#define DELAY 4000 //1000 -> sim, 2000 -> real
 
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void nothing(void);
void flash(void);
void Delay100ms(void);
void Delay(unsigned long i);
void Systick_init(void);
void PortF_Init(void);
void PortB_Init(void);
void PortE_Init(void);

// ***** 3. Subroutines Section *****
struct State{
	unsigned long name;
	unsigned long output;
	unsigned long next_state[8];
	unsigned long wait;
};
typedef struct State stateType;
stateType FSM[9]={
	{goWest, 	0x20C,	{goWest, 	goWest, 	hWest, 		hWest, 		hWest,	hWest,		hWest,		hWest			}, 	DELAY},
	{hWest,		0x214,	{hWest,		goWest,		goSth,		goSth,		Walk,		Walk,			Walk,			Walk			},	DELAY},
	{goSth,		0x221,	{goSth,		hSth,			goSth,		hSth,			hSth,		hSth,			hSth,			hSth			},	DELAY},
	{hSth,		0x222,	{hSth,		goWest,		goSth,		goWest,		Walk,		Walk,			Walk,			goWest		},	DELAY},
	{Walk,		0x824,	{Walk,	Fl_off1,	Fl_off1,	Fl_off1,	Walk,		Fl_off1,	Fl_off1,	Fl_off1		},	DELAY},
	{noWalk,	0x224,	{noWalk,	offWalk,	offWalk,	offWalk,	offWalk,offWalk,	offWalk,	offWalk	}, FLASH_DELAY},
	{offWalk,	0x024,	{offWalk,	Fl_on2,		Fl_on2,		Fl_on2,		Fl_on2,	Fl_on2,		Fl_on2,		Fl_on2	}, FLASH_DELAY},
	{Fl_on2,	0x224,	{Fl_on2,	Fl_off2,	Fl_off2,	Fl_off2,	Fl_off2,Fl_off2,	Fl_off2,	Fl_off2	}, FLASH_DELAY},
	{Fl_off2,	0x024,	{Fl_off2,	goWest,		goSth,		goSth,		Walk,		goWest,		goSth,		goSth		}, DELAY},
};
	unsigned long state;
	unsigned short sensor;
	unsigned short swap;
int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  EnableInterrupts();
	PortF_Init();
	PortB_Init();
	PortE_Init();
	Systick_init();
	state = goWest;
	sensor = 0x00;
  while(1){
		//set 6 leds
		GPIO_PORTB_DATA_R = (FSM[state].output) & 0x3F;
		//set pf3,pf1	
		GPIO_PORTF_DATA_R = ((FSM[state].output) & 0x00000A00)>>8;
		//wait
		Delay(FSM[state].wait);
		//input sensor
		sensor = GPIO_PORTE_DATA_R & 0x0F;
		//next state
    state = FSM[state].next_state[sensor];
  }
}

void Systick_init(){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 0x00ffffff;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x00000005;
	
}
void Delay100ms(){
	NVIC_ST_RELOAD_R = 800000;
	NVIC_ST_CURRENT_R = 0;
	while((NVIC_ST_CTRL_R & 0x00010000) == 0){
	}
}
void Delay(unsigned long i){
	unsigned long m = i /100;
	while(m){
		Delay100ms();
		m--;
	}
}
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  //GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
	GPIO_PORTF_PUR_R = 0x10;
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}
void PortB_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTB_CR_R = 0x3F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTB_DIR_R = 0x3F;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTB_PUR_R = 0x00;
  GPIO_PORTB_DEN_R = 0xFF;          // 7) enable digital I/O on PF4-0
}
void PortE_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTE_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTE_DIR_R = 0x00;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTE_PUR_R = 0x10;
  GPIO_PORTE_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}
