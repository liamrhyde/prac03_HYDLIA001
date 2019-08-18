/*
 * BinClock.c
 * Jarrod Olivier
 * Modified for EEE3095S/3096S by Keegan Crankshaw
 * August 2019
 * 
 * <HYDLIA001> <MYRMOE002>
 * Date:16/08/2019
*/
//test 1
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions

#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance
int arrLEDS[] = {38,37,36,35,32,31,15,13,11,7};
int arrBTNS[] = {18,16};


int HH,MM,SS;

wiringPiISR(18 INT_EDGE_RISING, &minInc);
wiringPiISR(16, INT_EDGE_RISING, &hourInc);

void initGPIO(void){
	/* 
	 * Sets GPIO using wiringPi pins. see pinout.xyz for specific wiringPi pins
	 * You can also use "gpio readall" in the command line to get the pins
	 * Note: wiringPi does not use GPIO or board pin numbers (unless specifically set to that mode)
	 */
	printf("Setting up\n");
	wiringPiSetup(); //This is the default mode. If you want to change pinouts, be aware
	
	RTC = wiringPiI2CSetup(RTCAddr); //Set up the RTC
	
	//Set up the LEDS
	for(int i; i < sizeof(arrLEDS)/sizeof(arrLEDS[0]); i++){
	    pinMode(arrLEDS[i], OUTPUT);
	}
	
	//Set Up the Seconds LED for PWM
	//Write your logic here
    pinMode(38, PWM_OUTPUT);
	
	printf("LEDS done\n");
	
	//Set up the Buttons
	for(int j; j < sizeof(arrBTNS)/sizeof(arrBTNS[0]); j++){
		pinMode(arrBTNS[j], INPUT);
		pullUpDnControl(arrBTNS[j], PUD_DOWN);
	}
	
	//Attach interrupts to Buttons
	//Write your logic here
	
	printf("BTNS done\n");
	printf("Setup done\n");
}


/*
 * The main function
 * This function is called, and calls all relevant functions we've written
 */
int main(void){
	initGPIO();

	//Set random time (3:04PM)
	//You can comment this file out later
	wiringPiI2CWriteReg8(RTC, HOUR, 0x13+TIMEZONE);
	wiringPiI2CWriteReg8(RTC, MIN, 0x4);
	wiringPiI2CWriteReg8(RTC, SEC, 0x00);
	
	// Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC
		//Write your logic here
		int time = wiringPiI2CRead(RTCAddr);                //Reads hours
        //int wiringPiI2CRead(RTCAddr);                //Reads min
        //int wiringPiI2CRead(RTCAddr);                //Reads seconds
		printf(time +"\n");
        //Function calls to toggle LEDs
		//Write your logic here
        //lightHours(HH);
        //lightMin(MM);
        //secPWM(SS);
        
        
        // Print out the time we have stored on our RTC
		printf("The current time is: %x:%x:%x\n", hours, mins, secs);

		//using a delay to make our program "less CPU hungry"
		delay(1000); //milliseconds
	}
	return 0;
}

/*
 * Change the hour format to 12 hours
 */
int hFormat(int hours){
	/*formats to 12h*/
	if (hours >= 24){
		hours = 0;
	}
	else if (hours > 12){
		hours -= 12;
	}
	return (int)hours;
}

/*
 * Turns on corresponding LED's for hours
 */
void lightHours(int units){
	// Write your logic to light up the hour LEDs here
    // i cant do this as i cant check the pins. same for the ones below.
	int h = hexCompensation(units);
	int arrHours[] = {0,0,0,0};
	int counter=0;
	while (h!=0){
		arrHours[3-counter] = h%2;
		h = (h - h%2)/2;
		counter++;
	}
	
	if (arrHours[0]==1){
		digitalWrite(15, HIGH);
	}
	else{
		digitalWrite(15, LOW);
	}
	
	if (arrHours[1]==1){
		digitalWrite(13, HIGH);
	}
	else{
		digitalWrite(13, LOW);
	}
	
	if (arrHours[2]==1){
		digitalWrite(11, HIGH);
	}
	else{
		digitalWrite(11, LOW);
	}
	
	if (arrHours[3]==1){
		digitalWrite(7, HIGH);
	}
	else{
		digitalWrite(7, LOW);
	}
	
}

/*
 * Turn on the Minute LEDs
 */
void lightMins(int units){
	//Write your logic to light up the minute LEDs here
		int h = hexCompensation(units);
	String leds="";
	int arrMinutes[] = {0,0,0,0,0,0};
	int counter=0;
	while (h!=0){
		arrHours[3-counter] = h%2;
		h = (h - h%2)/2;
		counter++;
	}
	
	if (arrMinutes[0]==1){
		digitalWrite(38, HIGH);
	}
	else{
		digitalWrite(38, LOW);
	}
	
	if (arrMinutes[0]==1){
		digitalWrite(37, HIGH);
	}
	else{
		digitalWrite(37, LOW);
	}
	
	if (arrMinutes[0]==1){
		digitalWrite(36, HIGH);
	}
	else{
		digitalWrite(36, LOW);
	}
	
	if (arrMinutes[0]==1){
		digitalWrite(35, HIGH);
	}
	else{
		digitalWrite(35, LOW);
	}
	
	if (arrMinutes[0]==1){
		digitalWrite(32, HIGH);
	}
	else{
		digitalWrite(32, LOW);
	}
	
	if (arrMinutes[0]==1){
		digitalWrite(31, HIGH);
	}
	else{
		digitalWrite(31, LOW);
	}
}

/*
 * PWM on the Seconds LED
 * The LED should have 60 brightness levels
 * The LED should be "off" at 0 seconds, and fully bright at 59 seconds
 */
void secPWM(int units){
	// Write your logic here
	pwmWrite(38, (int)(units*1024)/60);
}

/*
 * hexCompensation
 * This function may not be necessary if you use bit-shifting rather than decimal checking for writing out time values
 */
int hexCompensation(int units){
	/*Convert HEX or BCD value to DEC where 0x45 == 0d45 
	  This was created as the lighXXX functions which determine what GPIO pin to set HIGH/LOW
	  perform operations which work in base10 and not base16 (incorrect logic) 
	*/
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}


/*
 * decCompensation
 * This function "undoes" hexCompensation in order to write the correct base 16 value through I2C
 */
int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}


/*
 * hourInc
 * Fetch the hour value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 23 hours in a day
 * Software Debouncing should be used
 */
void hourInc(void){
	//Debounce
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 1 triggered, %x\n", hours);
		//Fetch RTC Time
        RTC = wiringPiI2CRead(RTCAddr);
		//Increase hours by 1, ensuring not to overflow
        HH++;
		printf("Hours =" HH);
        //if(decTime == 24){              //you need to check what dectime outputs, i dont know the format
        //    decTime = 0;
        //}
		//Write hours back to the RTC
        decCompensation(HH);          //converts dec back to hex
	}
	lastInterruptTime = interruptTime;
}

/* 
 * minInc
 * Fetch the minute value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 60 minutes in an hour
 * Software Debouncing should be used
 */
void minInc(void){
	
	long interruptTime = millis();
	
	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 2 triggered, %x\n", mins);
		//Fetch RTC Time
        RTC = wiringPiI2CRead(int fd);
		//Increase minutes by 1, ensuring not to overflow
        if (MM==60){
			MM=0;
			HH++;
		}	
		else{
			MM++;
		}
		printf("min =" MM);
		//Write minutes back to the RTC
        decCompensation(MM);
        
	}
	lastInterruptTime = interruptTime;
}

//This interrupt will fetch current time from another script and write it to the clock registers
//This functions will toggle a flag that is checked in main
void toggleTime(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		HH = getHours();
		MM = getMins();
		SS = getSecs();

		HH = hFormat(HH);
		HH = decCompensation(HH);
		wiringPiI2CWriteReg8(RTC, HOUR, HH);

		MM = decCompensation(MM);
		wiringPiI2CWriteReg8(RTC, MIN, MM);

		SS = decCompensation(SS);
		wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);

	}
	lastInterruptTime = interruptTime;
}

