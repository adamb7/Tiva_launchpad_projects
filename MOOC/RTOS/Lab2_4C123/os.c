// os.c
// Runs on LM4F120/TM4C123/MSP432
// Lab 2 starter file.
// Daniel Valvano
// February 20, 2016

#include <stdint.h>
#include "os.h"
#include "../inc/CortexM.h"
#include "../inc/BSP.h"

// function definitions in osasm.s
void StartOS(void);


tcbType tcbs[NUMTHREADS];
tcbType *RunPt;
int32_t Stacks[NUMTHREADS][STACKSIZE];
int32_t g_sema_mailbox;
int32_t g_mailbox;

// ******** OS_Init ************
// Initialize operating system, disable interrupts
// Initialize OS controlled I/O: systick, bus clock as fast as possible
// Initialize OS global variables
// Inputs:  none
// Outputs: none
void OS_Init(void){
  DisableInterrupts();
  BSP_Clock_InitFastest();// set processor clock to fastest speed
  // initialize any global variables as needed
  //***YOU IMPLEMENT THIS FUNCTION*****
	OS_MailBox_Init();
}

void SetInitialStack(int i){
  //***YOU IMPLEMENT THIS FUNCTION*****
	tcbs[i].sp=&Stacks[i][STACKSIZE-16];
	Stacks[i][STACKSIZE-1]=0x01000000;
	Stacks[i][STACKSIZE-3]=0x01000000;
	Stacks[i][STACKSIZE-4]=0x01000000;
	Stacks[i][STACKSIZE-5]=0x01000000;
	Stacks[i][STACKSIZE-6]=0x01000000;
	Stacks[i][STACKSIZE-7]=0x01000000;
	Stacks[i][STACKSIZE-8]=0x01000000;
	Stacks[i][STACKSIZE-9]=0x01000000;
	Stacks[i][STACKSIZE-10]=0x01000000;
	Stacks[i][STACKSIZE-11]=0x01000000;
	Stacks[i][STACKSIZE-12]=0x01000000;
	Stacks[i][STACKSIZE-13]=0x01000000;
	Stacks[i][STACKSIZE-14]=0x01000000;
	Stacks[i][STACKSIZE-15]=0x01000000;
	Stacks[i][STACKSIZE-16]=0x01000000;
}

//******** OS_AddThreads ***************
// Add four main threads to the scheduler
// Inputs: function pointers to four void/void main threads
// Outputs: 1 if successful, 0 if this thread can not be added
// This function will only be called once, after OS_Init and before OS_Launch
int OS_AddThreads(void(*thread0)(void),
                  void(*thread1)(void),
                  void(*thread2)(void),
                  void(*thread3)(void)){
	int32_t status;
// initialize TCB circular list
// initialize RunPt
// initialize four stacks, including initial PC
  //***YOU IMPLEMENT THIS FUNCTION*****
	status = StartCritical();
	tcbs[0].next = &tcbs[1];
	tcbs[1].next = &tcbs[2];
	tcbs[2].next = &tcbs[3];
	tcbs[3].next = &tcbs[0];									
	RunPt = &tcbs[0];
	SetInitialStack(0);
	SetInitialStack(1);
	SetInitialStack(2);
	SetInitialStack(3);
	Stacks[0][STACKSIZE-2] = (int32_t)(thread0);
	Stacks[1][STACKSIZE-2] = (int32_t)(thread1);
	Stacks[2][STACKSIZE-2] = (int32_t)(thread2);
	Stacks[3][STACKSIZE-2] = (int32_t)(thread3);
	EndCritical(status);
  return 1;               // successful
}

//******** OS_AddThreads3 ***************
// add three foregound threads to the scheduler
// This is needed during debugging and not part of final solution
// Inputs: three pointers to a void/void foreground tasks
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddThreads3(void(*task0)(void),
                 void(*task1)(void),
                 void(*task2)(void)){ 
// initialize TCB circular list (same as RTOS project)
// initialize RunPt
// initialize four stacks, including initial PC
  //***YOU IMPLEMENT THIS FUNCTION*****
  int32_t status;
	status = StartCritical();
	tcbs[0].next = &tcbs[1];
	tcbs[1].next = &tcbs[2];
	tcbs[2].next = &tcbs[0];
	RunPt = &tcbs[0];
	SetInitialStack(0);
	SetInitialStack(1);
	SetInitialStack(2);
	Stacks[0][STACKSIZE-2] = (int32_t)(task0);
	Stacks[1][STACKSIZE-2] = (int32_t)(task1);
	Stacks[2][STACKSIZE-2] = (int32_t)(task2);
	EndCritical(status);
  return 1;               // successful
}
                 
//******** OS_AddPeriodicEventThreads ***************
// Add two background periodic event threads
// Typically this function receives the highest priority
// Inputs: pointers to a void/void event thread function2
//         periods given in units of OS_Launch (Lab 2 this will be msec)
// Outputs: 1 if successful, 0 if this thread cannot be added
// It is assumed that the event threads will run to completion and return
// It is assumed the time to run these event threads is short compared to 1 msec
// These threads cannot spin, block, loop, sleep, or kill
// These threads can call OS_Signal
void (*gp_isr1)(void);
void (*gp_isr2)(void);
uint32_t g_isr1_period;
uint32_t g_isr2_period;
int OS_AddPeriodicEventThreads(void(*thread1)(void), uint32_t period1,
  void(*thread2)(void), uint32_t period2){
  //***YOU IMPLEMENT THIS FUNCTION*****
	gp_isr1 			= thread1;
	gp_isr2 			= thread2;
	g_isr1_period = period1;
	g_isr2_period = period2;
  return 1;
}

//******** OS_Launch ***************
// Start the scheduler, enable interrupts
// Inputs: number of clock cycles for each time slice
// Outputs: none (does not return)
// Errors: theTimeSlice must be less than 16,777,216
void OS_Launch(uint32_t theTimeSlice){
  STCTRL = 0;                  // disable SysTick during setup
  STCURRENT = 0;               // any write to current clears it
  SYSPRI3 =(SYSPRI3&0x00FFFFFF)|0xE0000000; // priority 7
  STRELOAD = theTimeSlice - 1; // reload value
  STCTRL = 0x00000007;         // enable, core clock and interrupt arm
  StartOS();                   // start on the first task
}
// runs every ms
void Scheduler(void){ // every time slice
  // run any periodic event threads if needed
  // implement round robin scheduler, update RunPt
  //***YOU IMPLEMENT THIS FUNCTION*****
	static uint32_t counter = 0;
	counter++;
	
	if(0 == counter%g_isr1_period){
		(*gp_isr1)();
	}
	if(0 == counter%g_isr2_period){
		(*gp_isr2)();
	}
	RunPt = RunPt->next;
}

// ******** OS_InitSemaphore ************
// Initialize counting semaphore
// Inputs:  pointer to a semaphore
//          initial value of semaphore
// Outputs: none
void OS_InitSemaphore(int32_t *semaPt, int32_t value){
  //***YOU IMPLEMENT THIS FUNCTION*****
	*semaPt = value;
}

// ******** OS_Wait ************
// Decrement semaphore
// Lab2 spinlock (does not suspend while spinning)
// Lab3 block if less than zero
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Wait(int32_t *semaPt){
	DisableInterrupts();
	while(0 == (*semaPt)){
		EnableInterrupts();
		DisableInterrupts();
	}
	(*semaPt) = (*semaPt) - 1;
	EnableInterrupts();
}

// ******** OS_Signal ************
// Increment semaphore
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Signal(int32_t *semaPt){
//***YOU IMPLEMENT THIS FUNCTION*****
	DisableInterrupts();
	(*semaPt) = (*semaPt) + 1;
	EnableInterrupts();
}




// ******** OS_MailBox_Init ************
// Initialize communication channel
// Producer is an event thread, consumer is a main thread
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void){
  // include data field and semaphore
  //***YOU IMPLEMENT THIS FUNCTION*****
	OS_InitSemaphore(&g_sema_mailbox,0);
	g_mailbox = 0;
}

// ******** OS_MailBox_Send ************
// Enter data into the MailBox, do not spin/block if full
// Use semaphore to synchronize with OS_MailBox_Recv
// Inputs:  data to be sent
// Outputs: none
// Errors: data lost if MailBox already has data
void OS_MailBox_Send(uint32_t data){
  //***YOU IMPLEMENT THIS FUNCTION*****
	if(g_sema_mailbox == 0){
		OS_Signal(&g_sema_mailbox);
		g_mailbox   		= data;
	}
}

// ******** OS_MailBox_Recv ************
// retreive mail from the MailBox
// Use semaphore to synchronize with OS_MailBox_Send
// Lab 2 spin on semaphore if mailbox empty
// Lab 3 block on semaphore if mailbox empty
// Inputs:  none
// Outputs: data retreived
// Errors:  none
uint32_t OS_MailBox_Recv(void){ uint32_t data;
  //***YOU IMPLEMENT THIS FUNCTION*****
	OS_Wait(&g_sema_mailbox);	
	data 			= g_mailbox;
	g_mailbox = 0;
  return data;
}
