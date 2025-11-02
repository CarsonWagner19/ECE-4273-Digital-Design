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
 *
 *
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdbool.h>


#define I2C0CONSET (*(volatile unsigned int *) 0x4001C000)
#define I2C0CONCLR (*(volatile unsigned int *) 0x4001C018)
#define I2C0SCLH (*(volatile unsigned int *) 0x4001C010)
#define I2C0SCLL (*(volatile unsigned int *) 0x4001C014)

#define PCONP (*(volatile unsigned int *) 0x400FC0C4)
#define I2C0DAT (*(volatile unsigned int *) 0x4001C008)


#define PINSEL1 (*(volatile unsigned int *)  0x4002C004)
#define PCLKSEL0 (*(volatile unsigned int *)0x400FC1A8)


int data;

int opcodeIO1 = 0b01001110; // IO1 Write
int opcodeIO2 = 0b01000000; // IO2 Write
int opcodeTempWrite = 0b10011100; // Temp Write OpCode
int opcodeTempRead = 0b10011101; // Temp Read OpCode


bool tempUnit = 1; // 1 Celsius, 0 Fahrenheit
int temp = 0;
int sw = 1;
int swPush;
int unit;


int tempArray[10];


void initializeI2C () {

	PCONP |= (1 << 7);
	PCLKSEL0 |= (1 << 14); // change to get desired frequency, pclk divider
	PCLKSEL0 |= (0 << 15);

	// Initializing SDA0 and SCL0 (Pin 25 and Pin 26)
	PINSEL1 &= ~(1 << 23);
	PINSEL1 |= (1 << 22);

	PINSEL1 &= ~(1 << 25);
	PINSEL1 |= (1 << 24);

	// I2C0SCLH and I2C0SCLL are Equal both are 5
	I2C0SCLH = 5; // duty cycle divider, must be 100khz by here
	I2C0SCLL = 5;


	// Clear and set I2C
	I2C0CONCLR = (1 << 6); // Clears I2C
	I2C0CONSET = (1 << 6); // Enables I2C
}


void initializeIO() {

	// Initialize IO1 A Register
	startI2C();
	dataWrite(opcodeIO1); // IO1 Address
	dataWrite(0x00); // IODIRA Register
	dataWrite(0x80); // Write A bits to be all outputs except Bit 7 (switch input) in GPA (1000 0000 in binary)
	stopI2C();

	// Initialize IO1 B Register
	startI2C();
	dataWrite(opcodeIO1); // IO1 Address
	dataWrite(0x01); // IODIRB Register
	dataWrite(0x00); // Write B bits to be all outputs
	stopI2C();

	// Initialize IO2 A Register
	startI2C();
	dataWrite(opcodeIO2); // IO2 Address
	dataWrite(0x00); // IODIRA Register
	dataWrite(0x00); // Set all A bits to outputs
	stopI2C();

	// Initialize IO2 B Register
	startI2C();
	dataWrite(opcodeIO2); // IO2 Address
	dataWrite(0x00); // IODIRB Register
	dataWrite(0x00); // Set all B bits to outputs
	stopI2C();

h

}


void initializeTempSensor() {

	startI2C();
	dataWrite(opcodeTempWrite); // Send Write OpCode for Sensor
	dataWrite(0x00); // Read Temp Address
	stopI2C();
}



void dataWrite(int data) {


	// Write Data to I2C then clear SI bit
	I2C0DAT = data;
	I2C0CONCLR = (1 << 3);

	while(!((I2C0CONSET >> 3) & 1)) {

	}

}

int dataRead() {


	// Clear SI Bit and wait for it to go back to 1
	I2C0CONCLR = (1 << 3);

	while(!((I2C0CONSET >> 3) & 1)) {

	}

	int data = I2C0DAT;
	return data;

}



int readSwitch() {

	int opcodeIO1Read = 0b01001111; // IO1 Read


	startI2C();
	dataWrite(opcodeIO1); // IO1 Write Address
	dataWrite(0x12); // GPIOA Register
	startI2C();
	I2C0CONCLR = (1 << 2); // Clear Ack bit
	dataWrite(opcodeIO1Read); // IO1 Read Address


	// Clear SI bit and wait for it to go back to 1
	I2C0CONCLR = (1 << 3);

		while(!((I2C0CONSET >> 3) & 1)) {

		}

	// Write Read data
	data = I2C0DAT;

	data = ((data >> 7) & 1); // Store Bit 7 to data

	stopI2C();

	return data;



}





int readTemp() {


	startI2C();
	dataWrite(opcodeTempWrite); // Write Register
	dataWrite(0x00); // Read Temperature Register
	startI2C();
	dataWrite(opcodeTempRead); // Read Register to read bit
	I2C0CONCLR = (1 << 2); // Clear AA bit


	temp = dataRead();

	stopI2C();

	return temp;


}

int segConvert(int digit) {

	switch (digit) {
		case 0:
			return 0b00111111; // Display 0
	    case 1:
	        return 0b00000110; // Display 1
	    case 2:
	        return 0b01011011; // Display 2
	    case 3:
	        return 0b01001111; // Display 3
	    case 4:
	        return 0b01100110; // Display 4
	    case 5:
	        return 0b01101101; // Display 5
	    case 6:
	        return 0b01111101; // Display 6
        case 7:
	        return 0b00000111; // Display 7
	    case 8:
            return 0b01111111; // Display 8
        case 9:
	        return 0b01101111; // Display 9
	    case 'C':
	        return 0b00111001; // Display C
	    case 'F':
	        return 0b01110001; // Display F
	     case '-':
	         return 0b01000000; // Display -
	     default:
	    	 return 0;
	}

}


void writeTemp(int temp) {



	// unitTemp = 1 (Celsius), unitTemp = 0;
	if(tempUnit == 0) {

		unit = segConvert('F'); // Selects F
	}
	else if(tempUnit == 1) {

		unit = segConvert('C');
	}

	// Check if 1, 2, or 3 digit value
	if(temp < 10) {

		int tempDigit = segConvert(temp);
		int digitZero = segConvert(0);
		int unit;


		writeDigitIO1(tempDigit, unit);
		writeDigitIO2(digitZero, digitZero);

	}
	else if(temp > 99) {

		// Calculate individual numbers
		int hundPlace = temp / 100;
		int tenthsPlace = (temp - 100) / 10;
		int onesPlace = (temp - 100) % 10;

		// Convert values to binary
		int digitZero = segConvert(0);
		int hundPlaceDigit = segConvert(hundPlace);
		int tenthsPlaceDigit = segConvert(tenthsPlace);
		int onesPlaceDigit = segConvert(onesPlace);


		writeDigitIO1(onesPlaceDigit, unit);
		writeDigitIO2(hundPlaceDigit, tenthsPlace);
	}
	else {

		int tenthsPlace = temp / 10;
		int onesPlace = temp % 10;

		int tenthsPlaceDigit = segConvert(tenthsPlace);
		int onesPlaceDigit = segConvert(onesPlace);
		int digitZero = segConvert(0);

		writeDigitIO1(onesPlaceDigit, unit);
		writeDigitIO2(digitZero, tenthsPlaceDigit);
	}



}


void writeDigitIO1(int right, int left) {



	startI2C();
	dataWrite(opcodeIO1); // Write IO1 Opcode
	dataWrite(0x12); // Write GPIOA register address
	dataWrite(left); // Give data for GPIOA
	dataWrite(right); // Writing GPIOB data automatically
	stopI2C();


}

void writeDigitIO2(int right, int left) {

	startI2C();
	dataWrite(opcodeIO2); // Write IO1 Opcode
	dataWrite(0x12); // Write GPIOA register address
	dataWrite(left); // Give data for GPIOA
	dataWrite(right); // Writing GPIOB data automatically
	stopI2C();


}

void startI2C () {


	// Clear and set I2C
	I2C0CONCLR = (1 << 6); // Clears I2C
	I2C0CONSET = (1 << 6); // Enables I2C


	I2C0CONSET = (1 << 3); // Set SI bit
	I2C0CONSET = (1 << 5); // Set Start Bit
	I2C0CONCLR = (1 << 3); // Clear SI bit


	// Wait for SI bit to go to 1
	while(!((I2C0CONSET >> 3) & 1)) {

	}

	// Clear Start Bit
	I2C0CONCLR = (1 << 5);
}



void stopI2C() {

	// Set STOP bit
	I2C0CONSET = (1 << 4);
	I2C0CONCLR = (1 << 3); // Clear SI Bit


	// Wait for STOP bit to be set
	while(((I2C0CONSET >> 4) & 1)) {

	}
}


// Function to create a delay in milliseconds
void wait_ms(int ms) {
    volatile int i;

    float m = 0.002715;
    float b = 0.1;

    ms = (ms-b)/m;

    for (i = 0; i < ms; i++) {
    //do nothing
    }

}


int main(void) {

	// Initialize I2C, IO1/IO2, and Temperature Sensor
    initializeI2C();
    initializeIO();
    initializeTempSensor();


    while(1) {


    	// Read and store temperature data
    	temp = (int) readTemp();

    	// Read switch and store data
    	swPush = readSwitch();


    	// Use for toggling
    	int swPressed = 1;

    	// If Switch is Pressed change to F
    	if(swPush == 0) {
    		tempUnit = !tempUnit; // Change to Fahrenheit (0) or Celsius (1)
    		swPressed = 0; // True, switch pressed

    		wait_ms(200);
    	}

    	//temp = (int) readTemp();


    	// If Unit is F, convert C to F (0 is F, 1 is C)
    	if(tempUnit == 0) {

    		temp = ((temp*9)/5) + 32;
    	}

    	writeTemp(temp);


    	// For toggling the switch to allow it to stay at Celsius or Fahrenheit
    	while((swPressed == 0) && (readSwitch() == 0))
    	{
    		// Wait until switch is pressed again
    	}

    }

    return 0 ;

}

