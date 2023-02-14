//Matthew Altmann
//altma094
//CSCI 2021 Assignment 2 Problem 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "batt.h"

int set_batt_from_ports(batt_t *batt){//initializes battery values
	if(BATT_VOLTAGE_PORT < 0){
		return 1;
	}
	batt->volts = BATT_VOLTAGE_PORT;
	if(batt->volts > 3800){
		batt->percent = 100;
	}
	else if(batt->volts < 3000){
		batt->percent = 0;
	}
	else{
		batt->percent = (batt->volts - 3000) / 8;//calculates battery percent
	}
	batt->mode = BATT_STATUS_PORT;
	return 0;
}



int set_display_from_batt(batt_t batt, int *display){
	*display = 0;//sets display to all zeroes
	int numarray[10] = {0b0111111, 0b0000011, 0b1101101, 0b1100111, 0b1010011, 0b1110110, 0b1111110, 0b0100011, 0b1111111, 0b1110111};//array of bit combinations that make up the numbers

	if(batt.percent > 89){//this block sets the battery indicator levels
		*display = 0b11111 << 24;
	}
	else if(69 < batt.percent && batt.percent < 89){
		*display = 0b11110 << 24;
	}
	else if(49 < batt.percent && batt.percent < 69){
		*display = 0b11100 << 24;
	}
	else if(29 < batt.percent && batt.percent < 49){
		*display = 0b11000 << 24;
	}
	else if(4 < batt.percent && batt.percent < 29){
		*display = 0b10000 << 24;
	}
	int dig1 = 0;
	int dig2 = 0;
	int dig3 = 0;
	short init_voltage = batt.volts;
	char init_percent = batt.percent;
	if(batt.mode == 0){//Voltage mode
		*display += 0b011 << 21;//sets Voltage indicator
		int dig4 = init_voltage % 10;//isolates fourth number
		if(dig4 >= 5){//rounding case
			dig3++;
		}
		init_voltage /= 10;
		dig3 += init_voltage % 10;//isolates third number
		init_voltage /= 10;
		dig2 = init_voltage % 10;//isolates second number
		init_voltage /= 10;
		dig1 = init_voltage;//isolates first number
		*display ^= numarray[dig1] << 14;//sets bit values and shifts them
		*display ^= numarray[dig2] << 7;
		*display ^= numarray[dig3];
	}
	else{//Percentage mode
		*display += 0b100 << 21;
		dig3 = init_percent % 10;//isolates third number
		init_percent /= 10;
		dig2 = init_percent % 10;//isolates second number
		init_percent /= 10;
		dig1 = init_percent;//isolates first number
		if(dig1 != 0){//if the percent is over 100
			*display ^= numarray[dig1] << 14;
			*display ^= numarray[0] << 7;
			*display ^= numarray[0];
		}
		else if(batt.percent == 0){
			*display ^= numarray[0];
		}
		else if(batt.percent < 10){
			*display ^= numarray[dig3];
		}
		else{
			*display ^= numarray[dig2] << 7;//sets bit values and shifts them
			*display ^= numarray[dig3];
		}
	}
	if(batt.volts < 0){
		return 1;
	}
	return 0;
}



int batt_update(){
	batt_t batt = {.volts=-100, .percent=-1, .mode=-1};//makes dummy battery
	int err = set_batt_from_ports(&batt);//sets battery from ports
	if(err != 1){
		set_display_from_batt(batt, &BATT_DISPLAY_PORT);//sets display from battery
		return 0;
	}
	return 1;
}
