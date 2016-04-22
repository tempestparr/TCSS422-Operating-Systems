/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#include "OS.h"
#include "PCB.h"
#include "FIFOq.h"

unsigned int SysStack[SYSSIZE];
int SysPointer;

void startCPU() {
    srand(time(NULL)); // seed random with current time
    //SysStack[SYSSIZE] = {UINT_MAX};
    SysPointer = -1;
    int base_error = 0;
    //other stuff
    if (mainLoopOS(&base_error)) {
        stackCleanup();
        if (EXIT_STATUS_MESSAGE) printf("System exited with error %d\n", base_error);
    } else {
        stackCleanup();
        if (EXIT_STATUS_MESSAGE) printf("System exited without incident\n");
    }
}

int mainLoopOS(int *error) {

    unsigned int PC = 0;
    unsigned int SW = 0;
    PCB_p current = NULL;
    PCB_p idl = PCB_construct_init(error);
    idl->pid = UINT_MAX;
//    idl->pc = 0x1000000;
    idl->priority = LOWEST_PRIORITY;
    idl->sw = UINT_MAX;
    idl->state = waiting;
    FIFOq_p createQ = FIFOq_construct(error);
    FIFOq_p readyQ = FIFOq_construct(error);
    int exit = 0;
    int i;
    

    
    if (*error) {
        printf("ERROR detected before launch! %d", *error);
        return *error;
    }
    
    while(true) {
        exit = createPCBs(createQ, error);
        scheduler(INTERRUPT_CREATE, createQ, readyQ, current, error);
                
        if (current == NULL) {
            idl->state = running;
            run(&PC, error);
            idl->state = waiting;
        } else {
            run(&PC, error);
        }
        SysStack[++SysPointer] = PC;
        SysStack[++SysPointer] = SW;
        
        isrTimer(createQ, readyQ, current, error);
        
        //other stuff for later
        
        if (*error) break;
        if (exit) break;
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
        if (EXIT_STATUS_MESSAGE) {
            printf("System exited with non-empty queue %s\n", qstr);
            printf("\t%s\n", FIFOq_toString(queue, str, &stz, error));
        }
        while (!FIFOq_is_empty(queue, error)) {
            PCB_p pcb = FIFOq_dequeue(queue, error);
            char pcbstr[PCB_TOSTRING_LEN];
            if (EXIT_STATUS_MESSAGE) printf("\t%s\n", PCB_toString(pcb, pcbstr, error));
            PCB_destruct(pcb);
        }
    }
    
    FIFOq_destruct(queue, error);
}

void stackCleanup() {
    int i;
    if (SysPointer >= 0) {
        if (EXIT_STATUS_MESSAGE) {
            printf("System exited with non-empty stack\n");
            for (i = 0; i <= SysPointer; i++) {
                printf("\tSysStack[%d] = %u\n", i, SysStack[i]);
            }
        }
    }
}

int createPCBs(FIFOq_p createQ, int *error) {
    int i;
    // random number of new processes between 0 and 5
    int r = rand() % (MAX_NEW_PCB+1);
    char buffer[PCB_TOSTRING_LEN];
    //static unsigned long pid = 0;
    static int processes_created = 0;
    if (r + processes_created >= MAX_PROCESSES) {
        r = MAX_PROCESSES - processes_created;
    }
    
    if (createQ == NULL) {
        *error += CPU_NULL_ERROR;
        printf("ERROR: FIFOq_p passed to createPCBs is NULL\n");
        return *error;
    }
    
    if (DEBUG) printf("createPCBs: creating %d PCBs and enqueueing them to createQ\n", r);
    for (i = 0; i < r; i++) {
        // PCB_construct initializes state to 0 (created)
        PCB_p newPcb = PCB_construct_init(error);
        //newPcb->state = created;
        // PCB_setPid(newPcb, pid++);
        if (DEBUG) printf("New PCB created: %s\n", PCB_toString(newPcb, buffer, error));
        FIFOq_enqueuePCB(createQ, newPcb, error);
        processes_created++;
    }
    
    return processes_created >= MAX_PROCESSES ? -1 : 0;
}

void run(unsigned int *PC, int *error) {
    int r = rand() % (RUN_TIME_RANGE+1) + RUN_MIN_TIME;
    *PC += r;
    if (DEBUG) printf("incrementing PC by %d. PC is now %p\n", r, (void *) PC);

}

// void run(PCB_p current, unsigned int *PC, int *error) {
//     if (current == NULL) {
//         *error += CPU_NULL_ERROR;
//         printf("ERROR: PCB_p passed to run is NULL\n");
//         return;
//     }
//     // increment the PC value by a random number between 3,000 and 4,000
//     int r = rand() % (RUN_TIME_RANGE+1) + RUN_MIN_TIME;
//     if (DEBUG) printf("run: running PID %u %d cycles\n", PCB_getPid(current, error), r);
//     *PC += r;
//     PCB_setPc(current, PCB_getPc(current, error) + r);
// }

void scheduler(const int INTERRUPT, FIFOq_p createQ, FIFOq_p readyQ, PCB_p current, int* error) {
	if (createQ == NULL) {
		*error += FIFO_NULL_ERROR;
		printf("%s", "ERROR: createQ is null");
		return;
	}
	if (readyQ == NULL) {
		*error += FIFO_NULL_ERROR;
		printf("%s", "ERROR: readyQ is null");
		return;
	}
	/*
	if (createQ->size == 0 && readyQ == 0) {
		current = idl;
	}*/
	
	while (createQ->size != 0) {
		PCB_p temp = PCB_construct(error);
		temp = FIFOq_dequeue(createQ, error);
		temp->state = ready;
		FIFOq_enqueuePCB(readyQ, temp, error);
		free(temp);
	}

	switch (INTERRUPT) {
		
		case NO_INTERRUPT:
			current->state = running;
			break;

		case INTERRUPT_CREATE:
			dispatcher(readyQ, current, error);
			break;

		case INTERRUPT_TIMER:
			current->state = ready;
			FIFOq_enqueuePCB(readyQ, current, error);
			current = NULL;
			dispatcher(readyQ, current, error);
			break;

		case INTERRUT_IO:
			current->state = halted;
			dispatcher(readyQ, current, error);
			break;
	}
}

void dispatcher(FIFOq_p readyQ, PCB_p current, int* error) {
	//save PC value into PCB
	PCB_setPc (current, current->pc);

	//dequeue the head of readyQueue
	FIFOq_dequeue(readyQ, error);

	//change PCB's state to running point
	PCB_setState (current, running);

	//copy PCB's PC value to SystemStack
	SysStack[++SysPointer] = current->pc;
}

void isrTimer(FIFOq_p createQ, FIFOq_p readyQ, PCB_p current, int* error) {
	//variables for big PC and big SW
	unsigned long bigPC = 0;
	unsigned long bigSW = 0;

	//change the state from running to interrupted
	PCB_setState (current, interrupted);

	//pop off SystemStack
	//create temporary PCB
	PCB_p tempHolder = PCB_construct(error);

	//pop off node on SystemStack and make temporary PCB it
//	tempHolder = FIFOq_dequeue(SysStack, error);
	//assigns Current PCB PC and SW values to popped values of SystemStack
	SysStack[SysPointer--] = current->pc;
	SysStack[SysPointer--] = current->sw;

	//delete this PCB
	free(tempHolder);

	//call Scheduler and pass timer interrupt parameter
	scheduler(INTERRUPT_TIMER, createQ, readyQ, current, error);

	//Scheduler sets Current PCB state to ready
	//PCB_setState (current, ready);
	//found in scheduler function of CPU.c

	//move it back to ReadyQueue
	//found in scheduler function of CPU.c
	//while the queue is not empty
	/*while (createQ->size != 0)
	{
		//create temporary PCB
		PCB_p temp = PCB_construct(error);

		//make temporary PCB the recently dequeued node
		temp = FIFOq_dequeue(createQ, error);

		//change PCB state to ready
		temp->state = ready;

		//copy temporary PCB into queue
		FIFOq_enqueuePCB(readyQ, temp, error);

		//delete temporary PCB
		free(temp);
	}*/


	//make Current pointer to IDLE or NULL
	//PCB_setState (current, halted);

	//Scheduler calls dispatcher
	dispatcher(readyQ, current, error);

	//take PCB's PC and SW values and push them into SystemStack
	SysStack[++SysPointer] = current->pc;
	SysStack[++SysPointer] = current->sw;

	//Scheduler does housekeeping
	//does nothing

	//Interrupt pops SystemStack and assign SW and PC values to bigSW and bigPC
	//
}