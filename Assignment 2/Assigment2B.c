#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include <stdbool.h>

//initialize registers
#define FIO0DIR (*(volatile unsigned int *)0x2009c000)	//blue; 00, 06, 07, 08, 09
#define FIO0PIN (*(volatile unsigned int *)0x2009c014) 	//green; 01, 15, 16, 17, 18
														//red; 02, 03, 21, 22, 27
														//score; 23, 24, 25, 26

#define FIO2DIR (*(volatile unsigned int *)0x2009c040)	//P1 switch; 00, 01, 02
#define FIO2PIN (*(volatile unsigned int *)0x2009c054) 	//P2 switch; 03, 04, 05

//initialize variables for wait function
float m = 0.002715;
float b = 0.1;

int countRed = 4;
int countGreen = 4;
int countBlue = 4;


//set blues function
void blue(int num){
	switch (num){
	case 0:
		FIO0PIN &= ~((1 << 9) | (1 << 8) | (1 << 7) | (1 << 6) | (1 << 0));
		break;
	case 1:
		FIO0PIN |= (1 << 9);
		break;
	case 2:
		FIO0PIN |= (1 << 9) | (1 << 8);
		break;
	case 3:
		FIO0PIN |= (1 << 9) | (1 << 8) | (1 << 7);
		break;
	case 4:
		FIO0PIN |= (1 << 9) | (1 << 8) | (1 << 7) | (1 << 6);
		break;
	case 5:
		FIO0PIN |= (1 << 9) | (1 << 8) | (1 << 7) | (1 << 6) | (1 << 0);
		break;
	default:
		for (int k = 0; k < 5; k++){
			FIO0PIN &= 0;
			wait_ms(5);
			FIO0PIN |= ~(0);
		}
		break;
	}
}

//set greens function
void green(int num){
	switch (num){
	case 0:
		FIO0PIN &= ~((1 << 1) | (1 << 18) | (1 << 17) | (1 << 15) | (1 << 16));
		break;
	case 1:
		FIO0PIN |= (1 << 1);
		break;
	case 2:
		FIO0PIN |= (1 << 1) | (1 << 18);
		break;
	case 3:
		FIO0PIN |= (1 << 1) | (1 << 18) | (1 << 17);
		break;
	case 4:
		FIO0PIN |= (1 << 1) | (1 << 18) | (1 << 17) | (1 << 15);
		break;
	case 5:
		FIO0PIN |= (1 << 1) | (1 << 18) | (1 << 17) | (1 << 15) | (1 << 16);
		break;
	default:
		for (int k = 0; k < 5; k++){
			FIO0PIN &= 0;
			wait_ms(5);
			FIO0PIN |= ~(0);
		}
		break;
	}
}

//set reds function
void red(int num){
	switch (num){
	case 0:
		FIO0PIN &= ~((1 << 2) | (1 << 3) | (1 << 21) | (1 << 22) | (1 << 27));
		break;
	case 1:
		FIO0PIN |= (1 << 2);
		break;
	case 2:
		FIO0PIN |= (1 << 2) | (1 << 3);
		break;
	case 3:
		FIO0PIN |= (1 << 2) | (1 << 3) | (1 << 21);
		break;
	case 4:
		FIO0PIN |= (1 << 2) | (1 << 3) | (1 << 21) | (1 << 22);
		break;
	case 5:
		FIO0PIN |= (1 << 2) | (1 << 3) | (1 << 21) | (1 << 22) | (1 << 27);
		break;
	default:
		for (int k = 0; k < 5; k++){
			FIO0PIN &= 0;
			wait_ms(5);
			FIO0PIN |= ~(0);
		}
		break;
	}
}


bool switchOnePressed() {

	return (((FIO2PIN >> 0) & 1) == 0); // P2.0 is Low

}

bool switchTwoPressed() {

	return (((FIO2PIN >> 1) & 1) == 0); // P2.1 is Low

}

bool switchThreePressed() {

	return (((FIO2PIN >> 2) & 1) == 0); // P2.2 is Low

}

bool switchFourPressed() {

	return (((FIO2PIN >> 3) & 1) == 0); // P2.3 is Low

}

bool switchFivePressed() {

	return (((FIO2PIN >> 4) & 1) == 0); // P2.4 is Low

}

bool switchSixPressed() {

	return (((FIO2PIN >> 5) & 1) == 0); // P2.5 is Low

}

// Function to create a delay in milliseconds
void wait_ms(int ms) {
    volatile int i;

    ms = (ms-b)/m;

    for (i = 0; i < ms; i++) {
    	//do nothing
    }
}


int main(void) {
    printf("Assignment Two");
    	FIO2DIR &= (0); //TODO
        FIO0DIR |= (1 << 0) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9)
        		 | (1 << 16) | (1 << 15) | (1 << 17) | (1 << 18) | (1 << 1)
				 | (1 << 27) | (1 << 22) | (1 << 21) | (1 << 3) | (1 << 2)
				 | (1 << 26) | (1 << 25) | (1 << 24) | (1 << 23);

// Enter an infinite loop
	while (1) {
		//flash to initiate game
		for (int j = 0; j < 6; j++){
			blue(j);
			wait_ms(500);
			green(j);
			wait_ms(500);
			red(j);
			wait_ms(500);

		}

		int totalLightsOn = 15;

		while(totalLightsOn = 15) {

			if(switchOnePressed() || switchFourPressed()) {
				red(countRed);
				wait_ms(500);

				countRed--;
				totalLightsOn--;
			}
			else if(switchTwoPressed() || switchFivePressed()) {
				green(countGreen);
				wait_ms(500);

				countGreen--;
				totalLightsOn--;
			}
			else if(switchThreePressed() || switchSixPressed()) {
				blue(countBlue);
				wait_ms(500);

				countBlue--;
				totalLightsOn--;
			}

			totalLightsOn = 15;

		}






//		FIO0PIN |= ~(0);
//		wait_ms(5);
//		FIO0PIN &= 0;
//		printf("Assignment Two");
	}
}


