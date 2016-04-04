
/* 
 * File:   main.c
 * Author: chris
 *
 * Created on March 25, 2016, 4:57 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pcb.h"

#define data 
/*
 * 
 */
int main(int argc, char** argv)
{
  int error;
  char str[80];
  srand(time(NULL));
  PCB_p pcb0 = PCB_construct(&error);
  printf("%s\n", PCB_toString(pcb0, str, &error));
  PCB_init(pcb0);
  printf("%s\n", PCB_toString(pcb0, str, &error));
  
  PCB_destruct(pcb0);
  return (EXIT_SUCCESS);
}

