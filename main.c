
#include <stdio.h>
#include <stdlib.h>
#include "FIFOq.h"

/*
 * 
 */
int main(int argc, char** argv) {
    PCB_test_main();
    FIFOq_test_main();
    READYq_test_main();
    
    return 0;
}

int PCB_test_main() {}
int FIFOq_test_main()
{
    int error = 0;
    char str[256];
    FIFOq_p fiq = FIFOq_construct(&error);
    printf("!!! %s", FIFOq_toString(fiq, str, &error));
}
int READYq_test_main() {}


