/*
 * Copyright 2022 NXP
 * NXP confidential.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms.  By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to
 * be bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>


#define PCONP (*(volatile unsigned int *) 0x400FC0C4)
#define PCLKSEL0 (*(volatile unsigned int *)0x400FC1A8)

#define AD0CR (*(volatile unsigned int *)0x40034000)
#define AD0GDR (*(volatile unsigned int *) 0x40034004)

#define PINSEL1 (*(volatile unsigned int *)  0x4002C004)
#define PINSEL4 (*(volatile unsigned int *)0x4002C010)

#define PWM1TCR (*(volatile unsigned int *) 0x40018004)
#define PWM1MR0 (*(volatile unsigned int *) 0x40018018)
#define PWM1MR1 (*(volatile unsigned int *) 0x4001801C)
#define PWM1MCR (*(volatile unsigned int *) 0x40018014)
#define PWM1PCR (*(volatile unsigned int *) 0x4001804C)
#define PWM1LER (*(volatile unsigned int *) 0x40018050)


float dataADC;


void initializeConvert() {

	// Setting the A/D Converter power/clock control bit (Turning A-to-D on)
	PCONP |= (1 << 12); // Setting the A/D Converter power/clock control bit (Turning A-to-D on)

	// Enable the ADC by setting PDN Bit in AD0CR Register
	AD0CR |= (1 << 21);

	// Selecting AD0.0
	PINSEL1 |= (1 << 14);

	// Set AD0.0 to Input
	AD0CR |= (1 << 0);

	// Disable Burst Bit (Set to 0)
	AD0CR &= ~(1 << 16);


}


void initializePWM() {

	// PWM Initializing as down in Lecture Slides
	PWM1TCR = 0;
	PCLKSEL0 |= (1<<12); // PWM PCLK = CCLK/1
	PWM1MR0 = 80000; // 256 PCLK cycle period (8-bit equivalent)
	PINSEL4 |= (1<<0); // Configure P2.0 as PWM1.1
	PWM1MCR = (1<<1); // Reset counter on MR0 match
	PWM1PCR = (1<<9); // Single edge mode and enable PWM1.1 only
	PWM1TCR = (1<<0) | (1<<3); // Enable counter and PWM mode


}


unsigned int readConversion() {

	// Start A-to-D Conversion
	AD0CR |= (1 << 24);


	// Wait for Done bit to go to 1
	while(!((AD0GDR >> 31) & 1)) {
		// Do nothing
	}

	// Clear A-to-D Start
	AD0CR &= ~(1 << 24);

	// Read Converted Result
	int data = ((AD0GDR >> 4) & 0xFFF);

	return data;

}

int ticks;

// Function to create a delay in milliseconds
void wait_ms(float ms) {


	volatile float i;


	float m = 0.002719;
	float b = 0.1;

    ms = (ms-b)/m;

    for (i = 0; i < ms; i++) {
    //do nothing
    }
}


int main(void) {

	// Initialize ADC and PWM
   	initializeConvert();
	initializePWM();


    while(1) {

    	// Conversion and Calculation for PWM
    	dataADC = readConversion();
    	PWM1MR1 = ((204.75 + 0.05 * dataADC) * 80000/4095);
    	PWM1LER = (1 << 1);
    	wait_ms(1);

    }
    return 0 ;
}


