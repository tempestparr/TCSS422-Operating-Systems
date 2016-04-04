
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "FIFOq.h"
#include "PriorityQ.h"

/*
 * 
 */
int main(int argc, char** argv) {

    srand(time(NULL));
    
    // printf("starting PCB_test_main\n");
    // PCB_test_main(0, NULL);
    
    // printf("starting FIFOq_test_main\n");
    // FIFOq_test_main(0, NULL);
    
    printf("starting PriorityQ_test_main\n");
    PriorityQ_test_main();
    
    return 0;
}

void PCB_test_main(int argc, char** argv) {}
//void FIFOq_test_main(int argc, char** argv) {}
void READYq_test_main(int argc, char** argv) {}


