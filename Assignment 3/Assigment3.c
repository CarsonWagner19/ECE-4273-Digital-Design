/*
===============================================================================
 Name        : Carson Wagner, Carson Bolinger, Roberto Solis
 Author      :
 Version     :
 Copyright   : 2/13/2024
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>

// Main PLL and PLL0
#define CLKSRCSEL (*(volatile unsigned int *)0x400FC10C)
#define PLL0CFG ((*(volatile unsigned int *)0x400Fc084)
#define PLL0STAT ((*(volatile unsigned int *)0x400Fc088)
#define PLL0FEED (*(volatile unsigned int *)0x400FC08C)
#define CLKSRCSEL (*(volatile unsigned int *)0x400FC10C)

// Clock Divider
#define CCLKCFG (*(volatile unsigned int *)0x400FC104)
#define PINSEL3 (*(volatile unsigned int *)0x4002C00C)
#define CLKOUTCFG (*(volatile unsigned int *)0x400FC1C8)

// Initialize Switch to change Frequency from 10 MHz to 8 MHz
#define FIO2DIR (*(volatile unsigned int *)0x2009c040) // Switch; 00
#define FIO2PIN (*(volatile unsigned int *)0x2009c054) //


void pllSetup()
{

	// Disconnecting the PLL
	PLL0CON &= ~(1 << 1);

	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;


	// Disabling PLL
	PLL0CON &= ~(1 << 0);
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;


	// Setup the Clock Divider
	CCLKCFG = 1 - 1;

	// Select OSC_CLK (External Crystal)
	CLKSRCSEL = 1;

	int m = 12; // M = 12 from calculations

	PLL0FG = m - 1; // (m - 1)
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;


	// Enable the PLL
	PLL0CON |= (1 << 0);
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;

	// Wait for PLL0 to check if it is Locked
	while(((PLL0STAT >> 26) & 1) == 0){

		// do nothing until it locks
	}


	// Initializing sysCLK and K we found in calculations
	int sysCLK = 120000000;
	k = 28.8;

	// Dividing 12 MHz/28.8 = 10 MHz square wave
	//CCLKFG = sysCLK/k;
	CCLKFG =


	// Connecting PLL0
	PLL0CON |= (1 << 1);
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;

}


// Function to create a delay in milliseconds
void wait_ms(int ms) {
    volatile int i;

    ms = (ms-b)/m;

    for (i = 0; i < ms; i++) {
    //do nothing
    }
}





// TODO: insert other definitions and declarations here

int main(void) {

    // Bits of PINSEL3 set to 10
	PINSEL3 &= ~(1 << 23);
	PINSEL3 |= (1 << 22);
	CLKOUTCFG |= (1 << 8);


	// Call Setup Method for PLL0
	pllSetup();



    while(1) {
        // Do nothing
    }
    return 0 ;
}


