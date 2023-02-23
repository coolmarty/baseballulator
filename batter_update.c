//Matthew Altmann
//altma094
//CSCI 2021 Assignment 2 Problem 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "batter.h"

int set_batter_from_ports(batter_t *batter){//initializes batterery values
	if(BATTER_SCORE_PORT < 0){
		return 1;
	}
	batter->war = BATTER_SCORE_PORT;
  //batter->average == APIget
	//batter->on_base_percentage == APIget

	batter->mode = BATTER_STATUS_PORT; //could modify for Injured / inactive / relegated
	return 0;
}



int set_display_from_batter(batter_t batter, int *display){
	*display = 0;//sets display to all zeroes
	int numarray[10] = {0b0111111, 0b0000011, 0b1101101, 0b1100111, 0b1010011, 0b1110110, 0b1111110, 0b0100011, 0b1111111, 0b1110111};//array of bit combinations that make up the numbers
  int leader = batter->war / 1;
	float trailer = batter->war % 1;
	if(trailer > .89){//this block sets the batterery indicator levels
		*display = 0b11111 << 24;
	}
	else if(.69 < trailer && trailer < .89){
		*display = 0b11110 << 24;
	}
	else if(49 < trailer && trailer < .69){
		*display = 0b11100 << 24;
	}
	else if(29 < trailer && trailer < 49){
		*display = 0b11000 << 24;
	}
	else if(4 < trailer && trailer < 29){
		*display = 0b10000 << 24;
	}
	int dig1 = 0;
	int dig2 = 0;
	int dig3 = 0;
	short init_voltage = batter.volts;
	char init_percent = batter.percent;
	if(batter.mode == 0){//Voltage mode
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
		else if(batter.percent == 0){
			*display ^= numarray[0];
		}
		else if(batter.percent < 10){
			*display ^= numarray[dig3];
		}
		else{
			*display ^= numarray[dig2] << 7;//sets bit values and shifts them
			*display ^= numarray[dig3];
		}
	}
	if(batter.volts < 0){
		return 1;
	}
	return 0;
}



int batter_update(){
	batter_t batter = {.volts=-100, .percent=-1, .mode=-1};//makes dummy batterery
	int err = set_batter_from_ports(&batter);//sets batterery from ports
	if(err != 1){
		set_display_from_batter(batter, &BATTER_DISPLAY_PORT);//sets display from batterery
		return 0;
	}
	return 1;
}
