/*
 * Problem 1 - OS Abstract Data Types
 * TCSS 422 A Spring 2016
 * Christopher Ottersen, Tempest Parr, Mark Peters
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pcb.h"
#include "FIFOq.h"
#include "PriorityQ.h"

int main(int argc, char** argv) {
	srand(time(NULL));
    
	// printf("\nstarting PCB_test_main\n");
	// PCB_test_main(0, NULL);
    
 	 printf("\nstarting FIFOq_test_main\n");
 	 FIFOq_test_main(0, NULL);
    
   	printf("\nstarting PriorityQ_test_main\n");
   	PriorityQ_test_main();
    
    return 0;
}