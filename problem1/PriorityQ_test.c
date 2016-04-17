/* 
 * File:   PriorityQ.c
 * Author: Tempest Parr
 *
 * Created on April 3rd 2016
 */
 
#include "PriorityQ.h"

int PriorityQ_test_main() {
    int error = 0;
    PriorityQ_p readyQ = PriorityQ_construct(&error);
    PCB_p pcbToEnqueue;
    PCB_p pcbDequeued;
    char strBuffer[80];
    int i, j, k, kLimit;
    
    // printf("Empty readyQ:\n%s", PriorityQ_toString(readyQ));
    for(i = 0; i < 10; i++) {
        for(j = 0; j < 10; j++) {
            pcbToEnqueue = PCB_construct(&error);
            PCB_init(pcbToEnqueue);
            PriorityQ_enqueue(readyQ, pcbToEnqueue, &error);
        }
        printf("\nreadyQ after %d enqueues:\n%s", (i + 1) * 10, PriorityQ_toString(readyQ, &error));
        
        kLimit = (rand() % 3) + 4;
        printf("\n");
        for(k = 0; k < kLimit; k++) {
            pcbDequeued = PriorityQ_dequeue(readyQ, &error);
            printf("%s\n", PCB_toString(pcbDequeued, strBuffer, &error));
        }

        printf("\nreadyQ after %d dequeues:\n%s", kLimit, PriorityQ_toString(readyQ, &error));
    }
    
    printf("\nerror is %d\n", error);
    PriorityQ_destruct(readyQ, &error);
    return (EXIT_SUCCESS);

}
