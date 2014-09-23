// main.c
// Runs on LM4F120/TM4C123


// this connection occurs in the USB debugging cable
// U0Rx (PA0) connected to serial port on PC
// U0Tx (PA1) connected to serial port on PC
// Ground connected ground in the USB cable

#include "UART.h"
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

void EnableInterrupts(void);  // Enable interrupts

 
void PortF_Init(void);

int main(void){ 
	
	unsigned long m;
  
  UART_Init();              // initialize UART
	EnableInterrupts();      
  
  PortF_Init();

	//port f -- pin 4 and pin 3 are connected to 1 motor and pin 2 and pin 1 are connected to the other.
	while(1){
   
		m = UART_InChar();//serial command getitng received 
		
		if(m == 'f'){
			
			GPIO_PORTF_DATA_R = 0x14;//forward instruction for the motors
			
		
		}
		if(m == 'b'){
			
			GPIO_PORTF_DATA_R = 0x0A;//back instruction for the motors
			
		
		}
		if(m == 'l'){
			
			GPIO_PORTF_DATA_R = 0x0C;//left instruction for the motors
			
		
		}
		 if(m == 'r'){
			
			GPIO_PORTF_DATA_R = 0x12;//right instruction for the motors 
			
		
		}
	 	if(m == 's'){
			
			GPIO_PORTF_DATA_R = 0x00;
			
		}
	}
}
	
//function to enable port F 
void PortF_Init(){unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000020; 			//system clock assigned to port F
	delay = SYSCTL_RCGC2_R;				//allow time for the clock to start
	        
	GPIO_PORTF_AMSEL_R = 0x00;			//disable analog on port F 
	GPIO_PORTF_PCTL_R = 0x00000000;			//PCTL GPIO on PF4-0
	GPIO_PORTF_DIR_R = 0x1E;			//PF4 to PF1 Output and PF0 input
	GPIO_PORTF_AFSEL_R = 0x0;			//disable alt funct on PF7-0
	GPIO_PORTF_DEN_R = 0x1F;			//enable digital I/O on PF4-0
	GPIO_PORTF_PUR_R = 0x00;			//disable pull-up in port F
	GPIO_PORTF_DATA_R = 0x04;			//default value is assigned to on only PF2
}


  
  


