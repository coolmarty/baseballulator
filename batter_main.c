// batter_sim.c:
//
// batterery meter simulator main program and supporting functions. Read
// voltage sensor value and mode (volts or percent) from the command
// line and show the results of running functions from batter_update.c
// on the screen.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "batter.h"

#define VOLTAGE_FLAG 0
#define PERCENT_FLAG 1

int main(int argc, char **argv){

  if(argc < 3){
    printf("usage: %s {voltage_val} {V | P}\n",argv[0]);
    printf("  arg1 voltage_val: integer, units of 0.001 volts \n");
    printf("  arg2 V or P: Voltage or Percent display\n");
    return 0;
  }
  BATTER_STATUS_PORT = atoi(argv[1]);
  //BATTER_SCORE_PORT = 0;
  printf("BATTER_STATUS_PORT set to: %u\n", BATTER_STATUS_PORT);

  // unsigned int batter_max = 1000 * 128;
  // if(batter_VOLTAGE_PORT > batter_max){
  //   printf("Voltage value %u exceeds max %u\n",batter_VOLTAGE_PORT,batter_max);
  //   return 1;
  // }

  if(argv[2][0]=='V' || argv[2][0]=='v'){
    BATTER_STATUS_PORT |= VOLTAGE_FLAG;
  }
  else if(argv[2][0]=='P' || argv[2][0]=='p'){
    BATTER_STATUS_PORT |= PERCENT_FLAG;
  }
  else{
    printf("Unknown display mode: '%s'\n",argv[2]);
    printf("Should be 'V' or 'P'\n");
    return 1;
  }

  batter_t batter = {.volts=-100, .percent=-1, .mode=-1};
  int result = set_batter_from_ports(&batter);
  printf("set_batter_from_ports(&batter );\n");

  printf("batter is {\n");
  printf("  .volts   = %d\n", batter.volts);
  printf("  .percent = %d\n", batter.percent);
  printf("  .mode    = %d\n", batter.mode);
  printf("}\n");

  // int quo = batter.volts / 1000;
  // int rem = batter.volts % 1000;
  // printf("Simulated volts is: %d.%d V\n",quo,rem);

  if(result != 0){
    printf("set_batter_from_ports() returned non-zero: %d\n",result);
    return 1;
  }

  printf("\nChecking results for display bits\n");

  int display = 0;
  result = set_display_from_batter(batter, &display);
  printf("set_display_from_batter(batter, &display);\n");

  printf("\ndisplay is:\n");
  printf("        3         2         1         0\n");
  printf("index: 10987654321098765432109876543210\n");
  printf("bits:  "); showbits(display); printf("\n");
  printf("guide:  |    |    |    |    |    |    |\n");
  printf("index:  30        20        10        0\n");

  if(result != 0){
    printf("set_display_from_batter() returned non-zero: %d\n",result);
    return 1;
  }

  printf("\nRunning batter_update()\n");

  batter_update();

  printf("\nbatter_DISPLAY_PORT is:\n");
  printf("index:  3         2         1    0    0\n");
  printf("index: 10987654321098765432109876543210\n");
  printf("bits:  "); showbits(BATTER_DISPLAY_PORT); printf("\n");
  printf("guide:  |    |    |    |    |    |    |\n");
  printf("index:  30        20        10        0\n");


  printf("\nbatterery Meter Display:\n");
  print_batter_display();

  return 0;
}
