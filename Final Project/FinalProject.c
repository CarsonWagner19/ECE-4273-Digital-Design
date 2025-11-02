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

// FOR LCD
#define FIO0DIR (*(volatile unsigned int *)0x2009c000) // Used for E and RS pin of LCD Display
#define FIO0PIN (*(volatile unsigned int *)0x2009c014)


// FOR ANALOG-TO-DIGITAL CONVERSION
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
int count = 0;

// Port 0.18, 17,15, 16, 25, 26, 21
int DB[] = {18, 17, 15, 16, 24, 25, 26, 2};

// Used to start game
int start = 1;


void initializeLCD() {

	// Initializing P0.0 (RS), P0.1 (E), and P0.6 (R/W) to be outputs
	FIO0DIR |= (1 << 0); // RS
	FIO0DIR |= (1 << 1); // E
	FIO0DIR |= (1 << 6); // R/W

	// Drive RS, E, and R/W to Low
	FIO0PIN &= ~(1 << 0); // RS
	FIO0PIN &= ~(1 << 1); // E
	FIO0PIN &= ~(1 << 6); // R/W


	// Setting DB Array Elements to be outputs corresponding with DB0 - DB7 pins of LCD display
	for(int i = 0; i < 8; i++) {

		FIO0DIR |= (1 << DB[i]);
	}


	wait_ms(4);


	// Write LCD Commands (0x38, 0x06, 0x0c, 0x01)
	LCDwriteCommand(0x38); // Use full 8-bit bus
	LCDwriteCommand(0x06); // Automatically Advances Cursor
	LCDwriteCommand(0x0c); // Enable Display Only (Cursor Off)
	LCDwriteCommand(0x01); // Clear Display and Move Cursor to Left


	// Wait 4 milliseconds
	wait_ms(4);

}


void LCDwriteCommand(int command) {


	// Update DB0-DB7
	for(int i = 0; i < 8; i++) {

		if((command >> i) & 1) {
			FIO0PIN |= (1 << DB[i]);
		}
		else {
			FIO0PIN &= ~(1 << DB[i]);
		}
	}

	// Drive R/W to Low
	FIO0PIN &= ~(1 << 6); // R/W

	// Drive RS Low
	FIO0PIN &= ~(1 << 0); // RS

	// Drive E to High then Low
	FIO0PIN |= (1 << 1);
	wait_ms(0.1);
	FIO0PIN &= ~(1 << 1);

	// Wait 100 microseconds (0.1 ms)
	wait_ms(0.1);

}


void LCDwriteData(int data) {

	// Update DB0-DB7 to match ACII Code
	for(int i = 0; i < 8; i++){

		if((data >> i) & 1){
			FIO0PIN |= (1 << DB[i]);
		}

		else{
			FIO0PIN &= ~(1 << DB[i]);
		}
	}

	// Drive R/W to Low
	FIO0PIN &= ~(1 << 6);

	// Drive RS High
	FIO0PIN |= (1 << 0);

	// Drive E High then Low
	FIO0PIN |= (1 << 1);
	wait_ms(0.1);
	FIO0PIN &= ~(1 << 1);

	// Wait 100 microseconds (0.1 ms)
	wait_ms(0.1);

}


initializeCustom() {

	// Custom Character C for "Circuit"
	LCDwriteCommand(0x40);
	LCDwriteData(0b11111); //Byte 1
	LCDwriteData(0b10000); //Byte 2
	LCDwriteData(0b10000);
	LCDwriteData(0b10000);
	LCDwriteData(0b10000);
	LCDwriteData(0b10000);
	LCDwriteData(0b10000);
	LCDwriteData(0b11111); //Byte 8

	LCDwriteCommand(0x80); // Position 0x80
	//Show character 0
	LCDwriteData(0x00);

	// Display "ircuit" with normal characters
	LCDwriteCommand(0x81);
	LCDwriteData(0x49); // I


	LCDwriteCommand(0x82);
	LCDwriteData(0x52); // R

	LCDwriteCommand(0x83);
	LCDwriteData(0x43); // C

	LCDwriteCommand(0x84);
	LCDwriteData(0x55); // U

	LCDwriteCommand(0x85);
	LCDwriteData(0x49); // I

	LCDwriteCommand(0x86);
	LCDwriteData(0x54); // T

	LCDwriteCommand(0x87);
	LCDwriteData(0x10); // Space between words


	// Custom Character D for "Dash"
	LCDwriteCommand(0x48);
	LCDwriteData(0b11110); //Byte 1
	LCDwriteData(0b10001); //Byte 2
	LCDwriteData(0b10001);
	LCDwriteData(0b10001);
	LCDwriteData(0b10001);
	LCDwriteData(0b10001);
	LCDwriteData(0b10001);
	LCDwriteData(0b11110); //Byte 8
	LCDwriteCommand(0x80);

	LCDwriteCommand(0x88); // Put in position 0x88
	//Show character 1
	LCDwriteData(0x01);



	//LCDwriteCommand(0x88);
	//LCDwriteData(0x44); // D

	LCDwriteCommand(0x89);
	LCDwriteData(0x41); // A

	LCDwriteCommand(0x8A);
	LCDwriteData(0x53); // S

	LCDwriteCommand(0x8B);
	LCDwriteData(0x48); // H


}

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


int main(void) {


	FIO0DIR &= (1 << 28); // Joystick Switch set to Input


	// Initialize Display
	initializeLCD();

	// Display "Circuit Dash" on LCD
	initializeCustom();


	// Initialize ADC and PWM
	initializeConvert();
	initializePWM();

	// Array to go through positions of 3rd row to make objects moves across LCD
	int objectPos[20] = {0xA7, 0xA6, 0xA5, 0xA4, 0xA3, 0xA2, 0xA1, 0xA0, 0x9F, 0x9E, 0x9D, 0x9C, 0x9B, 0x9A, 0x99,
			0x98, 0x97, 0x96, 0x95, 0x94};

	int ballJump[2] = {0x99, 0xC5};

	int ball = 0x2E; // Ball Hex Code
	int obstacle = 0x5B; // Obstacle Character
	int empty = 0x10; //Empty Character

	int ifJump = 0; // Check if ball is in middle of jump


    // Enter an infinite loop, just incrementing a counter
    while(1) {

    	// Conversion and Calculation for PWM
    	dataADC = readConversion();
    	int checkJump = ((204.75 + 0.05 * dataADC) * 80000/4095);
    	PWM1LER = (1 << 1);
    	wait_ms(1);

		// Fill 99 with Ball Character
		LCDwriteCommand(ballJump[0]);
		LCDwriteData(ball); // Ball Character
		int onGround = 1;


		LCDwriteCommand(objectPos[count]);
		LCDwriteData(obstacle);
		wait_ms(5);


		// Using Joystick to make Ball Jump
		if(checkJump == 4000) {


			// Empty 99 (Ground) with Empty Character
			LCDwriteCommand(ballJump[0]);
			LCDwriteData(empty); // Empty Character
			wait_ms(10);
			onGround = 0;

			// Fill C5 (Above) with Ball Character
			LCDwriteCommand(ballJump[1]);
			LCDwriteData(ball); // Ball Character
			wait_ms(10);

			// Indicate Ball is in air
			onGround = 0;

			// Update to 1 to indicate ball is jumping
			ifJump = 1;

		}

		// Continue Writing Objects across Display
		LCDwriteCommand(objectPos[count]);
		LCDwriteData(empty);
		wait_ms(1);

		// Keep Ball in Air if Player Jumped
		if(ifJump == 1) {

			// Fill C5 (Above) with Ball Character
			LCDwriteCommand(ballJump[1]);
			LCDwriteData(ball); // Ball Character
			wait_ms(10);
		}


		LCDwriteCommand(objectPos[count + 1]);
		LCDwriteData(obstacle);
		wait_ms(1);



		// Keep Ball in Air if Player Jumped
		if(ifJump == 1) {

			// Fill C5 (Above) with Ball Character
			LCDwriteCommand(ballJump[1]);
			LCDwriteData(ball); // Ball Character
			wait_ms(10);

		}


		LCDwriteCommand(objectPos[count + 1]);
		LCDwriteData(empty);
		wait_ms(1);

		// Keep Ball in Air if Player Jumped
		if(ifJump == 1) {

			// Fill C5 (Above) with Ball Character
			LCDwriteCommand(ballJump[1]);
			LCDwriteData(ball); // Ball Character
			wait_ms(10);


		}


		LCDwriteCommand(objectPos[count + 2]);
		LCDwriteData(obstacle);
		wait_ms(1);

		LCDwriteCommand(objectPos[count + 2]);
		LCDwriteData(empty);
		wait_ms(1);

		LCDwriteCommand(objectPos[count + 3]);
		LCDwriteData(obstacle);
		wait_ms(1);

		LCDwriteCommand(objectPos[count + 3]);
		LCDwriteData(empty);
		wait_ms(1);

		if(ifJump == 1) {

			// Empty C5 with Empty Character
			LCDwriteCommand(ballJump[1]);
			LCDwriteData(empty); // Empty Character
			wait_ms(10);

			// Update to 0 to indicate ball is no longer Jumping
			ifJump = 0;
		}

		count++;

		if(count > 18) {
			count = 0;
		}

    }
    return 0;

}

