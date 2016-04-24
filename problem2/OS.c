/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#include "OS.h"
#include "PCB.h"
#include "FIFOq.h"

/*extern global declarations for system stack*/
unsigned long SysStack[SYSSIZE];
int SysPointer;

/*OS only declarations for current and idle processes*/
PCB_p current;
PCB_p idl;

/* Launches the OS. Sets default values, initializes idle process and calls the
 * mainLoopOS to simulate running the cpu. Afterwards it cleans up reports
 * any errors encountered.
 */
void startOS() {
    srand(time(NULL)); // seed random with current time
    int base_error = 0;
    SysPointer = 0; //points at next unassigned stack item; 0 is empty
    
    idl = PCB_construct_init(&base_error);
    idl->pid = ULONG_MAX;
    idl->priority = LOWEST_PRIORITY;
    idl->sw = ULONG_MAX;
    idl->state = running;
    
    current = idl; //current's default state if no ready PCBs to run
    
    //begin system loop and print exit error messages
    mainLoopOS(&base_error);
            
    stackCleanup();
    
    if (base_error) {
        if (EXIT_STATUS_MESSAGE) printf("System exited with error %d\n", base_error);
        if (OUTPUT) printf("\nExit due to ERROR; see output\n");
    } else {
        if (EXIT_STATUS_MESSAGE) printf("System exited without incident\n");
        if (OUTPUT) printf("\n%d processes have been created so system has exited\n", MAX_PROCESSES);
    }

}

/* Main loop for the operating system. Initializes queues and PC/SW values.
 * Runs until exit is triggered (in assignment 2, this is when 30 PCBs have been
 * created). Repeatedly creates new PCBs, simulates running, gets interrupted
 * by the timer, and restarting the loop.
 * 
 * returns error
 */
int mainLoopOS(int *error) {

    unsigned long PC = 0; //we used a long to match the PCB value for PC
    unsigned long SW = 0;
//    unsigned long REGFILE[32] = {0};
    
    FIFOq_p createQ = FIFOq_construct(error);
    FIFOq_p readyQ = FIFOq_construct(error);
    
    int exit = 0;
    
    if (*error) {
        printf("ERROR detected before launch! %d", *error);
        return *error;
    }

    /**************************************************************************/
    /*************************** MAIN LOOP OS *********************************/
    /**************************************************************************/
    while(true) {
        
        exit = createPCBs(createQ, error);
                
        if (current == NULL) {
            *error += CPU_NULL_ERROR;
            printf("ERROR current process unassigned! %d", *error);

        } else {

            run(&PC, error);

            if (DEBUG) printf("\t\tStack going to push mainOS: %d\n", SysPointer);      
            SysStack[SysPointer++] = PC;
            SysStack[SysPointer++] = SW;

            isrTimer(createQ, readyQ, error);
            
            if (DEBUG) printf("\t\tStack going to pop mainOS: %d\n", SysPointer);
            SW = SysStack[--SysPointer];
            PC = SysStack[--SysPointer];

            if (DEBUG) printf("PC-----------------------------------PC: %lu\n", PC);

        }   
        
        if (*error) break;
        if (exit) break;
    }
    /**************************************************************************/
    /*************************** *********** **********************************/
    /**************************************************************************/    
    
    queueCleanup(readyQ, "readyQ", error);
    queueCleanup(createQ, "createQ", error);

    if (current != idl) {
        if (EXIT_STATUS_MESSAGE) {
            char pcbstr[PCB_TOSTRING_LEN];
            printf("System Idle: %s\n", PCB_toString(idl, pcbstr, error));
        }
        PCB_destruct(idl);
    }
    if (current != NULL) {
        if (EXIT_STATUS_MESSAGE) {
            char pcbstr[PCB_TOSTRING_LEN];
            printf("Running PCB: %s\n", PCB_toString(idl, pcbstr, error));
        }
        PCB_destruct(current);
    }
    
    return *error;
}

/* Simulates running cycles by incrementing the PC.
 */
void run(unsigned long *PC, int *error) {
    int r = rand() % (RUN_TIME_RANGE+1) + RUN_MIN_TIME;
    *PC += r;
    if (DEBUG) printf("incrementing PC by %d. PC is now %lu\n", r, *PC);
}

/* Interrupt service routine for the timer: interrupts the current PCB and saves
 * the CPU state to it before calling the scheduler.
 */
void isrTimer(FIFOq_p createQ, FIFOq_p readyQ, int* error) {
    //change the state from running to interrupted
    PCB_setState(current, interrupted);
    
    //assigns Current PCB PC and SW values to popped values of SystemStack
    if (DEBUG) printf("\t\tStack going to pop isrtimer: %d\n", SysPointer);
    current->sw = SysStack[--SysPointer];
    current->pc = SysStack[--SysPointer]; 
       
    //call Scheduler and pass timer interrupt parameter
    scheduler(INTERRUPT_TIMER, createQ, readyQ, error);
}

/* Always schedules any newly created PCBs into the ready queue, then checks
 * the interrupt type: if the interrupt is a timer interrupt, requeues the
 * currently running process in the ready queue and sets its state to ready.
 * Then calls the dispatcher.
 */
void scheduler(const int INTERRUPT, FIFOq_p createQ, FIFOq_p readyQ, int* error) {

    //for measuring every 4th output
    static int context_switch = 0;

    PCB_p temp;
                            
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
    
    //enqueue any created processes
    while (!FIFOq_is_empty(createQ, error)) {
        temp = FIFOq_dequeue(createQ, error);
        temp->state = ready;
        FIFOq_enqueuePCB(readyQ, temp, error);
        if (OUTPUT) {
            char pcbstr[PCB_TOSTRING_LEN];
            printf(">Enqueued to ready queue: %s\n", PCB_toString(temp, pcbstr, error));
        }
    }

    if (DEBUG) printf("createQ transferred to readyQ\n");
		
    //reroute based on interrupt
    switch (INTERRUPT) {

        case NO_INTERRUPT:
            current->state = running;
            break;

        case INTERRUPT_TIMER:
            context_switch++;
            
            if (!(context_switch % 4)) {
                char pcbstr[PCB_TOSTRING_LEN];
                printf(">PCB: %s\n", PCB_toString(current, pcbstr, error));        
                char rdqstr[PCB_TOSTRING_LEN];
                printf(">Switching to: %s\n", PCB_toString(readyQ->head->data, rdqstr, error));        
            }
            
            current->state = ready;
            if (current != idl)
                FIFOq_enqueuePCB(readyQ, current, error);
            else idl->state = waiting;
            dispatcher(readyQ, error);

            if (!(context_switch % 4)) {
                char runstr[PCB_TOSTRING_LEN];
                printf(">Now running: %s\n", PCB_toString(current, runstr, error));        
                char rdqstr[PCB_TOSTRING_LEN];
                printf(">Returned to ready queue: %s\n", PCB_toString(readyQ->tail->data, rdqstr, error));        
                int stz = 1024;
                char str[stz];
                printf(">%s\n", FIFOq_toString(readyQ, str, &stz, error));

            }

            break;

        case INTERRUPT_IO:
            current->state = halted;
            //IO stuff
            dispatcher(readyQ, error);
            break;
    }

    //"housekeeping"

}

/* Dispatches a new current process by dequeuing the head of the ready queue,
 * setting its state to running and copying its CPU state onto the stack.
 */
void dispatcher(FIFOq_p readyQ, int* error) {

    if (readyQ == NULL) {
	*error += FIFO_NULL_ERROR;
	printf("%s", "ERROR: readyQ is null");
    } else if (!FIFOq_is_empty(readyQ, error)) {	//dequeue the head of readyQueue
        current = FIFOq_dequeue(readyQ, error);
    } else {
        current = idl;
    }

    //change current's state to running point
    current->state = running;
    
    if (DEBUG) {
        char pcbstr[PCB_TOSTRING_LEN];
        printf("CURRENT: \t%s\n", PCB_toString(current, pcbstr, error));        
    }
    
    //copy currents's PC value to SystemStack
    if (DEBUG) printf("\t\tStack going to push dispatch: %d\n", SysPointer);
    SysStack[SysPointer++] = current->pc;
    SysStack[SysPointer++] = current->sw;

}

/* Creates 0 to 5 PCBs and enqueues them into a special queue, create queue.
 * Keeps track of how many PCBs have been created and sends an exit signal
 * when 30 have been created.
 */
int createPCBs(FIFOq_p createQ, int *error) {
    int i;
    // random number of new processes between 0 and 5
    int r = rand() % (MAX_NEW_PCB+1);
    char buffer[PCB_TOSTRING_LEN];
    static int processes_created = 0;
    
    if (r + processes_created >= MAX_PROCESSES) {
        r = MAX_PROCESSES - processes_created;
    }
    
    if (START_IDLE && processes_created == 0) {
        r = 0;
        processes_created += 1;
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
        if (DEBUG) printf("New PCB created: %s\n", PCB_toString(newPcb, buffer, error));
        FIFOq_enqueuePCB(createQ, newPcb, error);
        processes_created++;
    }
    if (DEBUG) printf("PCBs all created\n");
    return processes_created >= MAX_PROCESSES ? -1 : 0;
}

/* Deallocates the queue data structures on the C simulator running this program
 * so that we don't have memory leaks and horrible garbage.
 */
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
            if (pcb != idl) {
                char pcbstr[PCB_TOSTRING_LEN];
                if (EXIT_STATUS_MESSAGE) printf("\t%s\n", PCB_toString(pcb, pcbstr, error));
                PCB_destruct(pcb);
        
            }
    
        }
    }
    FIFOq_destruct(queue, error);
}

/* Deallocates the stack data structure on the C simulator running this program
 * so that we don't have memory leaks and horrible garbage.
 */
void stackCleanup() {
    int i;
    if (SysPointer > 0) {
        if (EXIT_STATUS_MESSAGE) {
            printf("System exited with non-empty stack\n");
            for (i = 0; i <= SysPointer; i++) {
                printf("\tSysStack[%d] = %lu\n", i, SysStack[i]);
            }
        }
    }
}
