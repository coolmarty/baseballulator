// batter_sim.c: DO NOT MODIFY
//
// batterery dispplay simulator support functions.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "batter.h"
#include <stdint.h>

#define batter_MAX_BITS 30

// Convenience struct for representing the batterery display  as characters
typedef struct {
  char chars[7][32];
} batter_display;

// Tied to the batterery, provides a measure of voltage in units of 0.01
// volts.
short BATTER_SCORE_PORT;

// Global variable used to control the batterery display. Making changes
// to this variable will change the batterery meter display. Type
// ensures 32 bits.
int32_t BATTER_DISPLAY_PORT = 0;

// Lowest order bit indicates whether display should be in Volts (0)
// or Percent (1). C code should only read this port. Tied to a user
// button which will toggle the bit.
unsigned char BATTER_STATUS_PORT = 0;

// +-^^^-+                         0
// |     |  #  #   ####  ####      1
// |     |  #  #      #     #      2
// |#####|  ####   ####  ####  V   3
// |#####|     #   #        #      4
// |#####|     # o ####  ####      5
// +-----+                         6
// 01234567890123456789012345678901
// 0         1         2         3

// position of decimal place is at row 4, col 17

batter_display init_display = {
  .chars = {
    "+-^^^-+                        ",
    "|     |                        ",
    "|     |                        ",
    "|     |                        ",
    "|     |                        ",
    "|     |                        ",
    "+-----+                        ",
  }
};

// Reset batter_display to be empty
void reset_batter_display(batter_display *batter){
  *batter = init_display;
}

// Print an batter_display
void internal_print_batter_display(batter_display *batter){
  for(int i=0; i<7; i++){
    printf("%s\n",batter->chars[i]);
  }
}

// Data and functions to set the state of the batter display

// Position and char in display
typedef struct {
  int r,c; int ch;
} charpos;

// Collection of characters corresponding to one bit in the state being set
typedef struct {
  int len;                      // number of chars corresponding to this bit
  charpos pos[5];               // position of chars for this bit
} charpos_coll;

// Correspondence of bit positions to which characters should be set
charpos_coll bits2chars[batter_MAX_BITS] = {
  { .len=4, .pos={{ 0,25,'#'},{ 1,25,'#'}, { 2,25,'#'},{ 3,25,'#'}} }, //  0
  { .len=4, .pos={{ 3,25,'#'},{ 4,25,'#'}, { 5,25,'#'},{ 6,25,'#'}} }, //  1
  { .len=4, .pos={{ 6,22,'#'},{ 6,23,'#'}, { 6,24,'#'},{ 6,25,'#'}} }, //  2
  { .len=4, .pos={{ 3,22,'#'},{ 4,22,'#'}, { 5,22,'#'},{ 6,22,'#'}} }, //  3
  { .len=4, .pos={{ 0,22,'#'},{ 1,22,'#'}, { 2,22,'#'},{ 3,22,'#'}} }, //  4
  { .len=4, .pos={{ 0,22,'#'},{ 0,23,'#'}, { 0,24,'#'},{ 0,25,'#'}} }, //  5
  { .len=4, .pos={{ 3,22,'#'},{ 3,23,'#'}, { 3,24,'#'},{ 3,25,'#'}} }, //  6

  { .len=4, .pos={{ 0,19,'#'},{ 1,19,'#'}, { 2,19,'#'},{ 3,19,'#'}} }, //  7
  { .len=4, .pos={{ 3,19,'#'},{ 4,19,'#'}, { 5,19,'#'},{ 6,19,'#'}} }, //  8
  { .len=4, .pos={{ 6,16,'#'},{ 6,17,'#'}, { 6,18,'#'},{ 6,19,'#'}} }, //  9
  { .len=4, .pos={{ 3,16,'#'},{ 4,16,'#'}, { 5,16,'#'},{ 6,16,'#'}} }, // 10
  { .len=4, .pos={{ 0,16,'#'},{ 1,16,'#'}, { 2,16,'#'},{ 3,16,'#'}} }, // 11
  { .len=4, .pos={{ 0,16,'#'},{ 0,17,'#'}, { 0,18,'#'},{ 0,19,'#'}} }, // 12
  { .len=4, .pos={{ 3,16,'#'},{ 3,17,'#'}, { 3,18,'#'},{ 3,19,'#'}} }, // 13

  { .len=4, .pos={{ 0,12,'#'},{ 1,12,'#'}, { 2,12,'#'},{ 3,12,'#'}} }, // 14
  { .len=4, .pos={{ 3,12,'#'},{ 4,12,'#'}, { 5,12,'#'},{ 6,12,'#'}} }, // 15
  { .len=4, .pos={{ 6, 9,'#'},{ 6,10,'#'}, { 6,11,'#'},{ 6,12,'#'}} }, // 16
  { .len=4, .pos={{ 3, 9,'#'},{ 4, 9,'#'}, { 5, 9,'#'},{ 6, 9,'#'}} }, // 17
  { .len=4, .pos={{ 0, 9,'#'},{ 1, 9,'#'}, { 2, 9,'#'},{ 3, 9,'#'}} }, // 18
  { .len=4, .pos={{ 0, 9,'#'},{ 0,10,'#'}, { 0,11,'#'},{ 0,12,'#'}} }, // 19
  { .len=4, .pos={{ 3, 9,'#'},{ 3,10,'#'}, { 3,11,'#'},{ 3,12,'#'}} }, // 20

  { .len=1, .pos={{ 6,14,'o'}}                                      }, // 21
  { .len=1, .pos={{ 3,28,'V'}}                                      }, // 22
  { .len=1, .pos={{ 4,28,'%'}}                                      }, // 23

  { .len=5,.pos={{1,1,'#'},{1,2,'#'},{1,3,'#'},{1,4,'#'},{1,5,'#'}} }, // 24
  { .len=5,.pos={{2,1,'#'},{2,2,'#'},{2,3,'#'},{2,4,'#'},{2,5,'#'}} }, // 25
  { .len=5,.pos={{3,1,'#'},{3,2,'#'},{3,3,'#'},{3,4,'#'},{3,5,'#'}} }, // 26
  { .len=5,.pos={{4,1,'#'},{4,2,'#'},{4,3,'#'},{4,4,'#'},{4,5,'#'}} }, // 27
  { .len=5,.pos={{5,1,'#'},{5,2,'#'},{5,3,'#'},{5,4,'#'},{5,5,'#'}} }, // 28
};


// Assumes ints are at least 32 bits for disp
void set_batter_display(batter_display *batter, int disp){
  int i,j;
  int mask = 0x1;
  reset_batter_display(batter);
  for(i=0; i<batter_MAX_BITS; i++){
    //    printf("Checking %d\n",i);
    if( disp & (mask << i) ){ // ith bit set, fill in characters
      //      printf("%d IS SET\n",i);
      charpos_coll coll = bits2chars[i];
      //      printf("Coll len: %d\n",coll.len);
      for(j=0; j<coll.len; j++){
        //        printf("Inner iter %d\n",j);
        charpos pos = coll.pos[j];
        batter->chars[pos.r][pos.c] = pos.ch;
        // print_batter_display(batter);
      }
    }
  }
}


// Use the global batter_DISPLAY_PORT to print the batterery display
void print_batter_display(){
  batter_display batter;
  set_batter_display(&batter, BATTER_DISPLAY_PORT);
  internal_print_batter_display(&batter);
  return;
}


// Print the most signficant (right-most) to least signficant bit in
// the integer passed in
#define INT_BITS 32
void showbits(int x){
  int i;
  int mask = 0x1;
  for(i=INT_BITS-1; i>=0; i--){
    int shifted_mask = mask << i;
    if(shifted_mask & x){
      putchar('1');
    } else {
      putchar('0');
    }
  }
  // Equivalent short version
  //    (x&(1<<i)) ? putchar('1'): putchar('0');
}
