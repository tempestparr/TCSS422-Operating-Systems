/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#include "CPU.h"
#include "PCB.h"
#include "FIFOq.h"

unsigned int SysStack[SYSSIZE];
int SysPointer;

void startCPU() {
    //SysStack[SYSSIZE] = {UINT_MAX};
    SysPointer = -1;
    int base_error = 0;
    //other stuff
    if (mainLoopOS(&base_error)) {
        stackCleanup();
        printf("System exited with error %d\n", base_error);
    } else {
        stackCleanup();
        printf("System exited without incident\n");
    }
}

int mainLoopOS(int *error) {

    unsigned int PC = 0;
    unsigned int SW = 0;
    PCB_p current = NULL;
    PCB_p idl = PCB_construct_init(error);
    idl->sw = UINT_MAX;
    idl->priority = LOWEST_PRIORITY;
    FIFOq_p createQ = FIFOq_construct(error);
    FIFOq_p readyQ = FIFOq_construct(error);
    int i;
    
    srand(time(NULL)); // seed random with current time
    
    if (*error) {
        printf("ERROR dectected before launch! %d", *error);
        return *error;
    }
    
    while(true) {
        createPCBs(createQ, error);
        //scheduler(INTERRUPT_CREATE, createQ, readyQ, current, error);
        
        // next line is debug code used to test run while scheduler isn't ready
        // current = FIFOq_dequeue(createQ, error);

        run(current, error);
        SysStack[++SysPointer] = PC;
        SysStack[++SysPointer] = SW;
        //isrTimer(scheduler(*)* function pointer, current, error);
        //other stuff for later
        break;
    }
    
    queueCleanup(readyQ, "readyQ", error);
    queueCleanup(createQ, "createQ", error);
    PCB_destruct(current);
    PCB_destruct(idl);
    
    return *error;
}

void queueCleanup(FIFOq_p queue, char *qstr, int *error) {
    int stz = 256;
    char str[stz];
    
    if (!FIFOq_is_empty(queue, error)) {
        printf("System exited with non-empty queue %s\n", qstr);
        printf("\t%s\n", FIFOq_toString(queue, str, &stz, error));
        while (!FIFOq_is_empty(queue, error)) {
            PCB_p pcb = FIFOq_dequeue(queue, error);
            char pcbstr[PCB_TOSTRING_LEN];
            printf("\t%s\n", PCB_toString(pcb, pcbstr, error));
            PCB_destruct(pcb);
        }
    }
    
    FIFOq_destruct(queue, error);
}

void stackCleanup() {
    int i;
    if (SysPointer >= 0) {
        printf("System exited with non-empty stack\n");
        for (i = 0; i <= SysPointer; i++) {
            printf("\tSysStack[%d] = %d\n", i, SysStack[i]);
        }
    }
}

void createPCBs(FIFOq_p createQ, int *error) {
    int i;
    // random number of new processes between 0 and 5
    int r = rand() % 6;
    char buffer[80];
    static unsigned long pid = 0;
    
    if (createQ == NULL) {
        *error = CPU_NULL_ERROR;
        printf("ERROR: FIFOq_p passed to createPCBs is NULL\n");
        return;
    }
    
    printf("createPCBs: creating %d PCBs and enqueueing them to createQ\n", r);
    for (i = 0; i < r; i++) {
        // PCB_construct initializes state to 0 (created)
        PCB_p newPcb = PCB_construct_init(error);
        // PCB_setPid(newPcb, pid++);
        printf("New PCB created: %s\n", PCB_toString(newPcb, buffer, error));
        FIFOq_enqueuePCB(createQ, newPcb, error);
    }
}

void run(PCB_p current, int *error) {
    if (current == NULL) {
        *error = CPU_NULL_ERROR;
        printf("ERROR: PCB_p passed to run is NULL\n");
        return;
    }
    // increment the PC value by a random number between 3,000 and 4,000
    int r = rand() % 1001 + 3000;
    printf("run: running PID %d %d cycles\n", PCB_getPid(current, error), r);
    PCB_setPc(current, PCB_getPc(current, error) + r);
}
