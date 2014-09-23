// UART.c
// Runs on TM4C123 or LM4F120
// Lab 11 involves switching this from UART1 to UART0.
//                 switching from PC5,PC4 to PA1,PA0
// Daniel Valvano
// August 3, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// this connection occurs in the USB debugging cable
// U0Rx (PA0) connected to serial port on PC
// U0Tx (PA1) connected to serial port on PC
// Ground connected ground in the USB cable

#include "tm4c123gh6pm.h"
#include "UART.h"


unsigned char String[10];

//------------UART_Init------------
// Initialize the UART for 115200 baud rate (assuming 80 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART_Init(void){
// as part of Lab 11, modify this program to use UART0 instead of UART1
//                 switching from PC5,PC4 to PA1,PA0
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // activate UART0
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // activate port A
  UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
  UART0_IBRD_R = 43;                    // IBRD = int(80,000,000 / (16 * 115200)) = int(43.402778)
  UART0_FBRD_R = 26;                    // FBRD = round(0.402778 * 64) = 26
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_CTL_R |= UART_CTL_UARTEN;       // enable UART
  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1,PA0
  GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1,PA0
                                        // configure PA1,PA0 as UART0
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA1,PA0
}

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
unsigned char UART_InChar(void){
// as part of Lab 11, modify this program to use UART0 instead of UART1
  while((UART0_FR_R&UART_FR_RXFE) != 0);
  return((unsigned char)(UART0_DR_R&0xFF));
}

//------------UART_InCharNonBlocking------------
// Get oldest serial port input and return immediately
// if there is no data.
// Input: none
// Output: ASCII code for key typed or 0 if no character
unsigned char UART_InCharNonBlocking(void){
// as part of Lab 11, modify this program to use UART0 instead of UART1
  if((UART0_FR_R&UART_FR_RXFE) == 0){
    return((unsigned char)(UART0_DR_R&0xFF));
  } else{
    return 0;
  }
}

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(unsigned char data){
// as part of Lab 11, modify this program to use UART0 instead of UART1
  while((UART0_FR_R&UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}

//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long UART_InUDec(void){
unsigned long number=0, length=0;
char character;
  character = UART_InChar();
  while(character != CR){ // accepts until <enter> is typed
// The next line checks that the input is a digit, 0-9.
// If the character is not 0-9, it is ignored and not echoed
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 4294967295
      length++;
      UART_OutChar(character);
    }
// If the input is a backspace, then the return number is
// changed and a backspace is outputted to the screen
    else if((character==BS) && length){
      number /= 10;
      length--;
      UART_OutChar(character);
    }
    character = UART_InChar();
  }
  return number;
}
//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(unsigned char buffer[]){
// as part of Lab 11 implement this function
	unsigned long i = 0;
		for(i = 0;buffer[i] != '\0';i++){
			UART_OutChar(buffer[i]);
		}
}


//-----------------------UART_ConvertUDec-----------------------
// Converts a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: store the conversion in global variable String[10]
// Fixed format 4 digits, one space after, null termination
// Examples
//    4 to "   4 "  
//   31 to "  31 " 
//  102 to " 102 " 
// 2210 to "2210 "
//10000 to "**** "  any value larger than 9999 converted to "**** "
void UART_ConvertUDec(unsigned long n){
// as part of Lab 11 implement this function
	
	unsigned long d;
	unsigned long e;
	unsigned long f;
	String[5] = 0;
	if(n < 10){
		String[1] = 0x20;
		String[0] = 0x20;
		String[2] = 0x20;
		String[3] = n + 0x30;
		String[4] = 0x20;
		return;
	}
	if(n >= 10000){
		String[4] = 0x20;
		String[3] = 0x2A;
		String[2] = 0x2A;
		String[1] = 0x2A;
		
		String[0] = 0x2A;
		return;
	}
	if(n > 9 && n <= 99){
		
		String[4] = 0x20;
		String[3] = (n % 10) + 0x30;
		d = n/10;
		String[2] = (d % 10) + 0x30;
		e = d/10;
		String[1] = 0x20;
		String[0] = 0x20;
		return;
	}
	if(n > 99 && n <= 999){
		
		String[4] = 0x20;
		String[3] = (n % 10) + 0x30;
		d = n/10;
		String[2] = (d % 10) + 0x30;
		e = d/10;
		String[1] = (e % 10) + 0x30;
		String[0] = 0x20;
		return;
	}
	if(n > 999 && n <= 9999){
		String[4] = 0x20;
		String[3] = (n % 10) + 0x30;
		d = n / 10;
		String[2] = (d % 10) + 0x30;
		e = d / 10;
		String[1] = (e % 10 ) + 0x30;
		f = e / 10;
		String[0] = (f % 10) + 0x30;
		return;
	}
	
  
  
}

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Fixed format 4 digits, one space after, null termination
void UART_OutUDec(unsigned long n){
  UART_ConvertUDec(n);     // convert using your function
  UART_OutString(String);  // output using your function
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 implement this function
	
	unsigned long d;
	unsigned long e;
	unsigned long f;
	String[8] = 0;//null
	String[7] = 0x6D;
	String[6] = 0x63;
	String[5] = 0x20;
	if(n == 0){
			String[4] = 0x30;
		String[3] = 0x30;
		String[2] = 0x30;
		String[1] = 0x2E;
		String[0] = 0x30;
		return;
	}
	if(n > 0 && n <= 9999){
		String[4] = (n % 10) + 0x30;
		d = n / 10;
		String[3] = (d % 10) + 0x30;
		e = d / 10;
		String[2] = (e % 10) + 0x30;
		String[1] = 0x2E;
		f = e / 10;
		String[0] = (f % 10) + 0x30;
		return;
	}
	if (n > 9999){
		String[4] = 0x2A;
		String[3] = 0x2A;
		String[2] = 0x2A;
		String[1] = 0x2E;
		String[0] = 0x2A;
		return;
 	}
	
  
}

//-----------------------UART_OutDistance-----------------------
// Output a 32-bit number in unsigned decimal fixed-point format
// Input: 32-bit number to be transferred (resolution 0.001cm)
// Output: none
// Fixed format 1 digit, point, 3 digits, space, units, null termination
void UART_OutDistance(unsigned long n){
  UART_ConvertDistance(n);      // convert using your function
	
  UART_OutString(String);       // output using your function
}
