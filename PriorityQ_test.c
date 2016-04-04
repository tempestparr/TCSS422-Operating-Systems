/* 
 * File:   PriorityQ.c
 * Author: Tempest Parr
 *
 * Created on April 3rd 2016
 */
 
#include "PriorityQ.h"

void PriorityQ_test_main() {
    
    PriorityQ_p readyQ = PriorityQ_construct();
    PCB_p pcbDequeued;
    char strBuffer[80];
    int i, j, k, kLimit;
    
    // printf("Empty readyQ:\n%s", PriorityQ_toString(readyQ));
    for(i = 0; i < 10; i++) {
        for(j = 0; j < 10; j++) {
            PriorityQ_enqueue(readyQ, PCB_construct(NULL));
        }
        printf("\nreadyQ after %d enqueues:\n%s", (i + 1) * 10, PriorityQ_toString(readyQ));
        
        kLimit = (rand() % 3) + 4;
        printf("\n");
        for(k = 0; k < kLimit; k++) {
            pcbDequeued = PriorityQ_dequeue(readyQ);
            printf("%s\n", PCB_toString(pcbDequeued, strBuffer, NULL));
        }

        printf("\nreadyQ after %d dequeues:\n%s", kLimit, PriorityQ_toString(readyQ));
    }
    
    // PriorityQ_enqueue(readyQ, PCB_construct(NULL));

    
    // printf("readyQ after 117 adds:\n%s", PriorityQ_toString(readyQ));
    // printf("PriorityQ_is_empty returns %d\n", PriorityQ_is_empty(readyQ));
    
    // pcbDequeued = PriorityQ_dequeue(readyQ);
    
    // printf("pcbDequeued is %d\n", pcbDequeued->pid);
    
    // printf("readyQ after dequeue:\n%s", PriorityQ_toString(readyQ));
    
    // PriorityQ_destruct(readyQ);
    // printf("readyQ is null aftrer destruct: %d\n", readyQ == NULL);
    
}