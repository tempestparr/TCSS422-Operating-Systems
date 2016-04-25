/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "PCB.h"
#include "FIFOq.h"
//#include "PriorityQ.h"
#include "OS.h"

#define WRITE_TO_FILE true

int main(int argc, char** argv) {
	
	if(WRITE_TO_FILE) {
		freopen("scheduleTrace.txt", "w", stdout);
	}
	
        startOS();
        
//	printf("\nstarting PCB_test_main\n");
//	PCB_test_main(0, NULL);
    
// 	printf("\nstarting FIFOq_test_main\n");
// 	FIFOq_test_main(0, NULL);
//    
//   	printf("\nstarting PriorityQ_test_main\n");
//   	PriorityQ_test_main();
    
    return 0;
}
