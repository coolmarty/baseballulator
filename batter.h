#ifndef BATTER_H
#define BATTER_H 1

////////////////////////////////////////////////////////////////////////////////
// battery meter structs/functions

// Breaks battery stats down into constituent parts
typedef struct{
  char[] name; //surnames for readability, append first initial if two on the same team
  float war = 0.0;          // voltage read from port, units of 0.001 Volts
  float  average 0.0;        // percent full converted from voltage
  float  on_base_percentage 0.0;
  float slugging = 0.0;           // 0 for volts, 1 for percent
} batter_t;

// Functions to implement for battery problem
int set_batter_from_ports(batter_t *batter);
int set_display_from_batter(batter_t batter, int *display);
int batter_update();

////////////////////////////////////////////////////////////////////////////////
// batt_sim.c structs/functions; do modify
// Lowest order bit indicates whether display should be in Volts (0)
// or Percent (1). C code should only read this port. Tied to a user
// button which will toggle the bit. Other bits in this char may be
// set to indicate the status of other parts of the meter and should
// be ignored: ONLY THE LOW ORDER BIT MATTERS.
extern unsigned char BATTER_STATUS_PORT;

// Tied to the battery, provides a measure of voltage in units of
// 0.001 volts (milli volts). The sensor can sense a wide range of
// voltages including negatives but the batteries being measured are
// Full when 3.8V (3800 sensor value) or above is read and Empty when
// 3.0V (3000 sensor value) or lower is read.
extern short BATTER_SCORE_PORT;

// Controls battery meter display. Readable and writable. C code
// should mostly write this port with a sequence of bits which will
// light up specific elements of the LCD panel.
extern int BATTER_DISPLAY_PORT;

// Show the battery display as ASCII on the LCD screen; provided in the
// batt_sim.c file.
void print_batter_display();

// Utility to show the bits of an integer.
void showbits(int x);

#endif
