
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "FIFOq.h"

/*
 * 
 */
int main(int argc, char** argv) {
    srand(time(NULL));
    PCB_test_main(0, NULL);
    FIFOq_test_main(0, NULL);
    READYq_test_main(0, NULL);
    
    return 0;
}

int PCB_test_main(int argc, char** argv) {}
//int FIFOq_test_main(int argc, char** argv) {}
int READYq_test_main(int argc, char** argv) {}


