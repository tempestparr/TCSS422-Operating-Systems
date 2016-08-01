/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "mutex.h"
#include "cond_var.h"
#include "PCB.h"
#include "PCB_Queue.h"
#include "PCB_Priority_Queue.h"

#define NO_DEADLOCKS 0

#define IDL_PID 0xFFFFFFFF

#define MAX_LOOPS 5000000

// time must be under 1 billion
#define SLEEP_TIME 900000 // 90000000
#define IO_TIME_MIN SLEEP_TIME * 3
#define IO_TIME_MAX SLEEP_TIME * 5

#define STARVATION_THRESHOLD 100000

#define MAX_PRIORITY_LEVELS 4				// max number of priortiy levels (0 - 5%, 1 - 80%, 2 - 10%, 3 - 5%)

#define MAX_IO_PROCESSES 49					// max number of io processes created
#define MAX_COMPUTE_INTENSIVE_PROCESSES 25	// max number of processes that do no request io or sync services
#define MAX_PRODUCER_CONSUMER_PAIRS 10		// max number of producer-consumer pairs
#define MAX_MUTUAL_RESOURCE_PAIRS 3

#define MAX_PRIORITY_0_PROCESSES 5
#define MAX_PRIORITY_1_PROCESSES 80
#define MAX_PRIORITY_2_PROCESSES 10
#define MAX_PRIORITY_3_PROCESSES 5

unsigned int pid_counter = 100;

enum INTERRUPT_TYPE {
	INTERRUPT_TYPE_TIMER, 
	INTERRUPT_TYPE_IO_A, 
	INTERRUPT_TYPE_IO_B,
    INTERRUPT_TYPE_INVALID // for first call to scheduler to move PCBs out of createdQs to readyQ
};

typedef struct {
	pthread_mutex_t* mutex;
	pthread_cond_t* condition;
	int flag;
	int sleep_length_min;
	int sleep_length_max;
} timer_arguments;

typedef timer_arguments* timer_args_p;

PCB_Queue_p createdQueue;
PCB_Priority_Queue_p readyQueue;
PCB_Queue_p terminatedQueue;
PCB_Queue_p waitingQueueA;
PCB_Queue_p waitingQueueB;
PCB_p currentPCB;
PCB_p idl;
unsigned long sysStack;
enum PCB_ERROR error = PCB_SUCCESS;
unsigned int total_processes_run_count = 0;

Mutex_p MutualResourceMutexes[MAX_MUTUAL_RESOURCE_PAIRS * 2];

// global integer for producer-consumer pairs to read from and write to
int producerConsumerVar[MAX_PRODUCER_CONSUMER_PAIRS];
Mutex_p producerConsumerMutexes[MAX_PRODUCER_CONSUMER_PAIRS];
Condition_p consumedCv[MAX_PRODUCER_CONSUMER_PAIRS];
Condition_p producedCv[MAX_PRODUCER_CONSUMER_PAIRS];

void* timer(void* arguments) {
	struct timespec sleep_time;
	timer_args_p args = (timer_args_p) arguments; 
    // printf("timer: before locking mutex\n");
	pthread_mutex_lock(args->mutex);
    // printf("timer: after locking mutex\n");
    sleep_time.tv_sec = 0;
	for(;;) {
		sleep_time.tv_nsec = args->sleep_length_min + rand() % (args->sleep_length_max - args->sleep_length_min + 1);
		nanosleep(&sleep_time, NULL);
		args->flag = 0;
		pthread_cond_wait(args->condition, args->mutex);
	}
}

/**
 * Check the ready queue to see if starvation is occurring, increment starvation counter of 
 * head nodes for each priority. 
 */
PCB_Queue_p starvationCheck(PCB_Priority_Queue_p readyQ, enum PCB_ERROR *error) {
	int i;
	for (i = 1; i < MAX_PRIORITY_LEVELS; i++) {

		if (!PCB_Queue_is_empty(readyQ->queues[i], error)) {
            
            PCB_p head = readyQ->queues[i]->first_node_ptr->value;
            
			if (head->starvation_quanta_count > STARVATION_THRESHOLD * i) {
				printf("Boosted priority %d\n", head->priority);
				PCB_print(head, error);
				head->priority_boost = 1;
			}

			if (head->priority_boost) {
				head->starvation_quanta_count = 0;
                return readyQ->queues[i];
				// PCB_p boostMe = PCB_Queue_dequeue(readyQ->queues[i], error);
				// PCB_Queue_enqueue(readyQ->queues[head->priority - 1], boostMe, error);
			}
		}
	}
    return NULL;
}

void incrementStarvationQuanta(PCB_Priority_Queue_p readyQ, enum PCB_ERROR *error) {
    int i;
	for (i = 1; i < MAX_PRIORITY_LEVELS; i++) {
		if (!PCB_Queue_is_empty(readyQ->queues[i], error)) {
			PCB_p head = readyQ->queues[i]->first_node_ptr->value;
			head->starvation_quanta_count++;
        }
    }
}

void dispatcher() {
    PCB_Queue_p starvedQueue;
	// if the ready queue is not empty
	if (get_PQ_size(readyQueue, &error) > 0) {        
        // if there is queue with a process that needs boosted, run that process  
        starvedQueue = starvationCheck(readyQueue, &error);

        if (starvedQueue != NULL) {
            currentPCB = PCB_Queue_dequeue(starvedQueue, &error);
            readyQueue->size--;
            currentPCB->priority_boost = 0;
        } else {
        	// dequeue from the ready queue
            currentPCB = PCB_Priority_Queue_dequeue(readyQueue, &error);
        }
        currentPCB->starvation_quanta_count = 0;

        if (!currentPCB->has_been_run) {
        	currentPCB->has_been_run = 1;
        	total_processes_run_count++;
        }

	// if the ready queue is empty, switch to the idle pcb
	} else {
		currentPCB = idl;
	}

	printf("\nSwitching to:\t");
	PCB_print(currentPCB, &error);
	printf("Ready Queue:\t");
	PCB_Priority_Queue_print(readyQueue, &error);

	// change the state of the pcb that was just dequeued from the
	// ready queue from ready to running
	PCB_set_state(currentPCB, PCB_STATE_RUNNING, &error);

	// push the pcb pc value to the sysStack to replace the interrupted process
	sysStack = PCB_get_pc(currentPCB, &error);
}

void scheduler(enum INTERRUPT_TYPE interruptType) {
	// handling processes that have run to completion
	while (!PCB_Queue_is_empty(terminatedQueue, &error)) {
		PCB_p p = PCB_Queue_dequeue(terminatedQueue, &error);

		PCB_p new = PCB_construct(&error);
		PCB_set_pid(new, pid_counter++, &error);
		PCB_set_priority(new, p->priority, &error);
		new->type = p->type;
		new->pair_id = p->pair_id;
		PCB_Queue_enqueue(createdQueue, new, &error);
		printf("Created:\t");
		PCB_print(new, &error);
		// will enter next while loop and schedule this process

		printf("Deallocated:\t");
		PCB_print(p, &error);
		PCB_destruct(p, &error);
	}

	// handling processes created for the first time
	// move processes from created queue into the ready queue
	while (!PCB_Queue_is_empty(createdQueue, &error)) {
		PCB_p p = PCB_Queue_dequeue(createdQueue, &error);
		PCB_set_state(p, PCB_STATE_READY, &error);
		// printf("Scheduled:\t");
		// PCB_print(p, &error);
		PCB_Priority_Queue_enqueue(readyQueue, p, &error);
	}

	// if timer interrupt occurs for the current process running
	if (interruptType == INTERRUPT_TYPE_TIMER) {
		// change the state of the current pcb from interrupted to ready
		PCB_set_state(currentPCB, PCB_STATE_READY, &error);

		// if the current pcb is not the idle process
		if (PCB_get_pid(currentPCB, &error) != IDL_PID) {
			printf("Returned:\t");
			PCB_print(currentPCB, &error);

			if (currentPCB->priority_boost) {
				currentPCB->priority_boost = 0;
			}

			// put the pcb in the back of the ready queue
			PCB_Priority_Queue_enqueue(readyQueue, currentPCB, &error);
		}

		// call to dispatcher
		dispatcher();

	// if io device A interrupts the current process running
	} else if (interruptType == INTERRUPT_TYPE_IO_A) {
		// dequeue pcb from wait queue A
		PCB_p p = PCB_Queue_dequeue(waitingQueueA, &error);
		printf("IO COMPLETION: Moved from IO A:\t");
		PCB_print(p, &error);

		// print wait queue A
		printf("Waiting Queue A:");
		PCB_Queue_print(waitingQueueA, &error);

		// set the state of pcb just dequeued to ready
		PCB_set_state(p, PCB_STATE_READY, &error);

		// enqueue the pcb into the ready queue
		PCB_Priority_Queue_enqueue(readyQueue, p, &error);

	// if io device B interrupts the current process running
	} else if (interruptType == INTERRUPT_TYPE_IO_B) {
		// dequeue pcb from wait queue B
		PCB_p p = PCB_Queue_dequeue(waitingQueueB, &error);
		printf("IO COMPLETION: Moved from IO B:\t");
		PCB_print(p, &error);

		// print wait queue B
		printf("Waiting Queue B:");
		PCB_Queue_print(waitingQueueB, &error);

		// set the state of pcb just dequeued to ready
		PCB_set_state(p, PCB_STATE_READY, &error);

		// enqueue the pcb into the ready queue
		PCB_Priority_Queue_enqueue(readyQueue, p, &error);
	}
}


// function for timer interrupts
void isrTimer() {
	// print the current process
	printf("\nTIMER INTERRUPT: Switching from:\t");
	PCB_print(currentPCB, &error);

	// change current process state from running to interrupted
	PCB_set_state(currentPCB, PCB_STATE_INTERRUPTED, &error);

	// then save the cpu state to the pbc
	PCB_set_pc(currentPCB, sysStack, &error);

	// call scheduler with a timer interrupt
	scheduler(INTERRUPT_TYPE_TIMER);
}

// function for blocking the process on failure to lock
void isrBlock() {
	// print the current process
	printf("\nBLOCK: Switching from:\t");
	PCB_print(currentPCB, &error);

	// then save the cpu state to the pbc
    // we want to retry the lock trap on first walk through the loop
	PCB_set_pc(currentPCB, sysStack - 1, &error);
	// call scheduler with a timer interrupt
	//scheduler(INTERRUPT_TYPE_TIMER);
	dispatcher();
}

void isrIO(enum INTERRUPT_TYPE interruptType) {
    // call scheduler so that it moves PCB from appropriate waitQ to readyQ
	scheduler(interruptType);
}

// process has terminated and needs to be put into the termination list
void terminate() {
	printf("\nTERMINATE: Switching from:\t");
	PCB_print(currentPCB, &error);
	PCB_set_pc(currentPCB, sysStack, &error);
	PCB_set_state(currentPCB, PCB_STATE_HALTED, &error);

	// set the process's termination to the computer clock time
	PCB_set_termination(currentPCB, time(NULL), &error);

	printf("Terminated:\t");
	PCB_print(currentPCB, &error);
	// add the process to the termination list
	PCB_Queue_enqueue(terminatedQueue, currentPCB, &error);

	// call to dispatcher
	dispatcher();
}

// function for io interrupts
void tsr(enum INTERRUPT_TYPE interruptType) {
	printf("\nIO TRAP HANDLER: Switching from:\t");
	PCB_print(currentPCB, &error);

	// change the current process state from running to waiting
	PCB_set_state(currentPCB, PCB_STATE_WAITING, &error);

	// then save the cpu state to the pcb
	PCB_set_pc(currentPCB, sysStack, &error);

	// if IO device A interrupts
	if (interruptType == INTERRUPT_TYPE_IO_A) {
		printf("Moved to IO A:\t");
		// move the current pcb into wait queue A
		PCB_Queue_enqueue(waitingQueueA, currentPCB, &error);
		PCB_print(currentPCB, &error);

		// print wait queue A
		printf("Waiting Queue A:");
		PCB_Queue_print(waitingQueueA, &error);

	// if IO device B interrupts
	} else if (interruptType == INTERRUPT_TYPE_IO_B) {
		printf("Moved to IO B:\t");
		// move the current pcb into wait queue B
		PCB_Queue_enqueue(waitingQueueB, currentPCB, &error);
		PCB_print(currentPCB, &error);

		// print wait queue B
		printf("Waiting Queue B:");
		PCB_Queue_print(waitingQueueB, &error);
	} 

	// call to dispatcher
	dispatcher();
}

/**
 * Determines the priority based on the pairflag.
 */
unsigned short determinePriority(char pairFlag) {
	static int pri1Count = 0;
	static int pri2Count = 0;
	static int pri3Count = 0;
	unsigned short priority;
	char okay = 0;
	if (pri1Count == MAX_PRIORITY_1_PROCESSES && pri2Count == MAX_PRIORITY_2_PROCESSES) {
		if (pairFlag) {
			pri3Count++;
		}
		pri3Count++;
		return 3;
	} else if (pri1Count == MAX_PRIORITY_1_PROCESSES && pri3Count == MAX_PRIORITY_3_PROCESSES) {
		if (pairFlag) {
			pri2Count++;
		}
		pri2Count++;
		return 2;
	}
	while (!okay) {
			okay = 1;
			priority = rand() % 19; // priorities 1, 2, or 3
			if (priority < 16) {
				priority = 1;
			} else if (priority < 18) {
				priority = 2;
			} else {
				priority = 3;
			}
			switch (priority) {
				case 1:
					if ((pairFlag == 1 && pri1Count >= (MAX_PRIORITY_1_PROCESSES - 1)) ||
					    (pairFlag == 0 && pri1Count == MAX_PRIORITY_1_PROCESSES)) {
						okay = 0;
						continue;
					}
					pri1Count++;
					if (pairFlag) {
						pri1Count++;
					}
					break;
				case 2:
					if ((pairFlag == 1 && pri2Count >= (MAX_PRIORITY_2_PROCESSES - 1)) ||
					    (pairFlag == 0 && pri2Count == MAX_PRIORITY_2_PROCESSES)) {
						okay = 0;
						continue;
					}
					pri2Count++;
					if (pairFlag) {
						pri2Count++;
					}
					break;
				case 3:
					if ((pairFlag == 1 && pri3Count >= (MAX_PRIORITY_3_PROCESSES - 1)) ||
					    (pairFlag == 0 && pri3Count == MAX_PRIORITY_3_PROCESSES)) {
						okay = 0;
						continue;
					}
					pri3Count++;
					if (pairFlag) {
						pri3Count++;
					}
			}
		}
		return priority;
}

/**
 * Sets up the trap steps needed for locking, unlocking, and trylocking mutexes.
 */
void setMutexTraps1221(PCB_p pcb, unsigned long step) {
	memcpy(pcb->lockR1,   ((unsigned long[4]) {1 * step, 5 * step,  9 * step, 13 * step}), 4 * sizeof(unsigned long)); //Steps where process will lock mutex R1
	memcpy(pcb->unlockR1, ((unsigned long[4]) {4 * step, 8 * step, 12 * step, 16 * step}), 4 * sizeof(unsigned long)); //Steps where process will unlock mutex R1
    memcpy(pcb->lockR2,   ((unsigned long[4]) {2 * step, 6 * step, 10 * step, 14 * step}), 4 * sizeof(unsigned long)); //Steps where process will lock mutex R2
	memcpy(pcb->unlockR2, ((unsigned long[4]) {3 * step, 7 * step, 11 * step, 15 * step}), 4 * sizeof(unsigned long)); //Steps where process will unlock mutex R2
}

/**
 * Sets up the trap steps needed for locking, unlocking, and trylocking mutexes.
 */
void setMutexTraps2112(PCB_p pcb, unsigned long step) {
	memcpy(pcb->lockR1,   ((unsigned long[4]) {2 * step, 6 * step, 10 * step, 14 * step}), 4 * sizeof(unsigned long)); //Steps where process will lock mutex R1
	memcpy(pcb->unlockR1, ((unsigned long[4]) {3 * step, 7 * step, 11 * step, 15 * step}), 4 * sizeof(unsigned long)); //Steps where process will unlock mutex R1
    memcpy(pcb->lockR2,   ((unsigned long[4]) {1 * step, 5 * step,  9 * step, 13 * step}), 4 * sizeof(unsigned long)); //Steps where process will lock mutex R2
	memcpy(pcb->unlockR2, ((unsigned long[4]) {4 * step, 8 * step, 12 * step, 16 * step}), 4 * sizeof(unsigned long)); //Steps where process will unlock mutex R2
}

/**
 * Sets up the trap steps needed for locking, unlocking, and trylocking mutexes.
 */
void setProducerConsumerTraps(PCB_p pcb, unsigned long step) {
	memcpy(pcb->lockR1,           ((unsigned long[4]) {1 * step, 5 * step,  9 * step, 13 * step}), 4 * sizeof(unsigned long)); //Steps where process will lock mutex R1
	memcpy(pcb->wait_condition,   ((unsigned long[4]) {2 * step, 6 * step, 10 * step, 14 * step}), 4 * sizeof(unsigned long)); //Steps where process will wait for cv
    memcpy(pcb->signal_condition, ((unsigned long[4]) {3 * step, 7 * step, 11 * step, 15 * step}), 4 * sizeof(unsigned long)); //Steps where process will signal cv
	memcpy(pcb->unlockR1,         ((unsigned long[4]) {4 * step, 8 * step, 12 * step, 16 * step}), 4 * sizeof(unsigned long)); //Steps where process will unlock mutex R1
}

/**
 * Creates the initial processes for the simulation.
 */
void createProcesses(PCB_Queue_p createdQ, enum PCB_ERROR *error) {

	int i;
    unsigned int producerConsumerPairCounter = 0;
    unsigned int mutualResourcePairCounter = 0;
    printf("Creating processes...\n");
	// create priority 0 intensive processes. 
	for (i = 0; i < MAX_PRIORITY_0_PROCESSES; i++) {
		PCB_p p = PCB_construct(error);
		PCB_set_pid(p, i, error);
		PCB_set_priority(p, 0, error);
		p->type = COMPUTE_INTENSIVE_PROCESS;
		p->max_pc += rand() % 5000;
		PCB_Queue_enqueue(createdQueue, p, error);
		// printf("Created Priority 0 Compute Intensive Processes:\n");
		// PCB_print(p, error);
	}
    
    // create producer/consumer pairs; SWITCH BACK TO 94
	for (i = 74; i < 94; i += 2) {
		PCB_p p = PCB_construct(error);
		PCB_p buddy = PCB_construct(error);
		unsigned short priority = determinePriority(1);
		PCB_set_pid(p, i, error);
		PCB_set_pid(buddy, i + 1, error);
		PCB_set_priority(p, priority, error);
		PCB_set_priority(buddy, priority, error);
		p->type = PRODUCER_PROCESS;
		buddy->type = CONSUMER_PROCESS;
        buddy->terminate = p->terminate;
		p->pair_id = producerConsumerPairCounter;
		buddy->pair_id = producerConsumerPairCounter;
		p->mate_pid = i + 1;
		buddy->mate_pid = i;
        
        producerConsumerVar[producerConsumerPairCounter] = producerConsumerPairCounter * 100;
        producerConsumerMutexes[producerConsumerPairCounter] = Mutex_construct(100 + producerConsumerPairCounter, error);
        consumedCv[producerConsumerPairCounter] = Condition_construct(100 + producerConsumerPairCounter, error);
        consumedCv[producerConsumerPairCounter]->flag = 1;
        producedCv[producerConsumerPairCounter] = Condition_construct(200 + producerConsumerPairCounter, error);
        producedCv[producerConsumerPairCounter]->flag = 0;
        
        setProducerConsumerTraps(p, p->max_pc / ((PCB_TRAP_LENGTH * 4) + 1));
        setProducerConsumerTraps(buddy, buddy->max_pc / ((PCB_TRAP_LENGTH * 4) + 1));
        
		PCB_Queue_enqueue(createdQueue, p, error);
		PCB_Queue_enqueue(createdQueue, buddy, error);
		// printf("Created Producer/Consumer Pair %d:\n", producerConsumerPairCounter);
		// PCB_print(p, error);
		// PCB_print(buddy, error);
		producerConsumerPairCounter++;
	}

    // create mutual resource pairs
	for (i = 94; i < 100; i += 2) {
		PCB_p p = PCB_construct(error);
		PCB_p buddy = PCB_construct(error);
		unsigned short priority = determinePriority(1);
		PCB_set_pid(p, i, error);
		PCB_set_pid(buddy, i + 1, error);
		PCB_set_priority(p, priority, error);
		PCB_set_priority(buddy, priority, error);
		p->type = MUTUAL_RESOURCE_PROCESS;
		buddy->type = MUTUAL_RESOURCE_PROCESS;
        // p->terminate = 17; // hacks
        buddy->terminate = p->terminate;
		p->pair_id = mutualResourcePairCounter;
		buddy->pair_id = mutualResourcePairCounter;
		p->mate_pid = i + 1;
		buddy->mate_pid = i;
        
        MutualResourceMutexes[2 * mutualResourcePairCounter] = Mutex_construct(2 * mutualResourcePairCounter, error);
        MutualResourceMutexes[2 * mutualResourcePairCounter + 1] = Mutex_construct(2 * mutualResourcePairCounter + 1, error);
        
        // pass the step as 17th part of maxpc, because we want to spread traps evenly from 0 to maxpc
#if (NO_DEADLOCKS == 1)
        setMutexTraps1221(p, p->max_pc / ((PCB_TRAP_LENGTH * 4) + 1));
        setMutexTraps1221(buddy, buddy->max_pc / ((PCB_TRAP_LENGTH * 4) + 1));
#else
        setMutexTraps1221(p, p->max_pc / ((PCB_TRAP_LENGTH * 4) + 1));
        setMutexTraps2112(buddy, buddy->max_pc / ((PCB_TRAP_LENGTH * 4) + 1));
#endif

		PCB_Queue_enqueue(createdQueue, p, error);
		PCB_Queue_enqueue(createdQueue, buddy, error);
		// printf("\nCreated Mutual Resource Pair %d:\n", mutualResourcePairCounter);
		// PCB_print(p, error);
		// PCB_print(buddy, error);
        mutualResourcePairCounter++;
	}

    // create remaining compute intensive processes
	for (i = 5; i < 25; i++) {
		PCB_p p = PCB_construct(error);
		PCB_set_pid(p, i, error);
		PCB_set_priority(p, determinePriority(0), error);
		p->type = COMPUTE_INTENSIVE_PROCESS;
		p->max_pc += rand() % 5000;
		PCB_Queue_enqueue(createdQueue, p, error);
		// printf("Created Compute Intensive Processes:\n:");
		// PCB_print(p, error);
	}

    // create io processes
	for (i = 25; i < 74; i++) {
		PCB_p p = PCB_construct(error);
		PCB_set_pid(p, i, error);
		PCB_set_priority(p, determinePriority(0), error);
		p->type = IO_PROCESS;
		PCB_Queue_enqueue(createdQueue, p, error);
		// printf("Created IO Processes:\n");
		// PCB_print(p, error);
	}

}

/**
 * Free up all memory resources used by the simulation.
 */
void destroy_everything(enum PCB_ERROR *error) {
	int i;

	for (i = 0; i < MAX_MUTUAL_RESOURCE_PAIRS * 2; i += 2) {
		Mutex_destruct(MutualResourceMutexes[i], error);
		Mutex_destruct(MutualResourceMutexes[i + 1], error);
	}

	for (i = 0; i < MAX_PRODUCER_CONSUMER_PAIRS; i++) {
		Mutex_destruct(producerConsumerMutexes[i], error);
		Condition_destruct(consumedCv[i], error);
		Condition_destruct(producedCv[i], error);
	}

	PCB_destruct(idl, error);
	if (currentPCB != idl) {
		PCB_destruct(currentPCB, error);
	}

	PCB_Priority_Queue_destruct(readyQueue, error);
	PCB_Queue_destruct(createdQueue, error);
	PCB_Queue_destruct(terminatedQueue, error);
	PCB_Queue_destruct(waitingQueueA, error);
	PCB_Queue_destruct(waitingQueueB, error);
}

int main() {
    
    int i = 0;
    unsigned long j = 0;
    char ioThreadACreated = 0;
    char ioThreadBCreated = 0;
    
	pthread_t       system_timer, io_timer_a, io_timer_b;
	pthread_mutex_t mutex_timer, mutex_io_a, mutex_io_b;
	pthread_cond_t  cond_timer, cond_io_a, cond_io_b;
	timer_args_p    system_timer_args, io_timer_a_args, io_timer_b_args;
	
	pthread_mutex_init(&mutex_timer, NULL);
	pthread_mutex_init(&mutex_io_a, NULL);
	pthread_mutex_init(&mutex_io_b, NULL);
	
	pthread_cond_init(&cond_timer, NULL);
	pthread_cond_init(&cond_io_a, NULL);
	pthread_cond_init(&cond_io_b, NULL);
	
	system_timer_args = malloc(sizeof(timer_arguments));
	io_timer_a_args =   malloc(sizeof(timer_arguments));
	io_timer_b_args =   malloc(sizeof(timer_arguments));
	
	system_timer_args->mutex = &mutex_timer;
	system_timer_args->condition = &cond_timer;
	system_timer_args->sleep_length_min = SLEEP_TIME;
	system_timer_args->sleep_length_max = SLEEP_TIME;
	system_timer_args->flag = 1;
	
	io_timer_a_args->mutex = &mutex_io_a;
	io_timer_a_args->condition = &cond_io_a;
	io_timer_a_args->sleep_length_min = IO_TIME_MIN;
	io_timer_a_args->sleep_length_max = IO_TIME_MAX;
	io_timer_a_args->flag = 1;
	
	io_timer_b_args->mutex = &mutex_io_b;
	io_timer_b_args->condition = &cond_io_b;
	io_timer_b_args->sleep_length_min = IO_TIME_MIN;
	io_timer_b_args->sleep_length_max = IO_TIME_MAX;
	io_timer_b_args->flag = 1;
	
	srand(time(NULL));
	
	if(pthread_create(&system_timer, NULL, timer, (void*) system_timer_args)) {
		printf("\nERROR creating timer thread");
		return 1;
	}

/* =================================================== */
	// create the idle process
	idl = PCB_construct(&error);
	PCB_set_pid(idl, IDL_PID, &error);
	PCB_set_state(idl, PCB_STATE_RUNNING, &error);
	PCB_set_terminate(idl, 0, &error);
	PCB_set_max_pc(idl, 0, &error);
	for (i = 0; i < PCB_TRAP_LENGTH; i++) {
		idl->io_1_traps[i] = 1;
		idl->io_2_traps[i] = 1;
	}
	currentPCB = idl;
/* =================================================== */

	createdQueue = PCB_Queue_construct(&error);
	waitingQueueA = PCB_Queue_construct(&error);
	waitingQueueB = PCB_Queue_construct(&error);
	readyQueue = PCB_Priority_Queue_construct(&error);
	terminatedQueue = PCB_Queue_construct(&error);

	createProcesses(createdQueue, &error);
    
    // this will move everything from createdQ to readyQ
    // then will switch from idle process to first process created
    isrTimer();

    // while the wait queues aren't empty, or the ready queue isn't empty, or the current pcb running isn't the idle process
    // basically, while there is a process that isn't done yet
	// while(!PCB_Queue_is_empty(waitingQueueA, &error) || !PCB_Queue_is_empty(waitingQueueB, &error) || get_PQ_size(readyQueue, &error) > 0 || currentPCB != idl) {
	for (j = 0; j < MAX_LOOPS; j++) {	

		if (error != PCB_SUCCESS) {
			printf("\nERROR: error != PCB_SUCCESS");
			return 1;
		}
		
        /*
         *
         *	CHECK FOR TIMER INTERRUPT
         *
		 * try to lock timer mutex
         * if we succeed, it means the timer is done with the lock and we now have the lock
         * if we don't succeed, it means timer is still nanosleeping, we move on
         */
		if(pthread_mutex_trylock(&mutex_timer) == 0) {

			// if flag is 0, timer is done nanosleeping
            if (system_timer_args->flag == 0) {
            	// set the flag to 1 to signal that the timer is nanosleeping now
                system_timer_args->flag = 1;

                // call isrTimer to switch to the next process
                isrTimer();
                pthread_cond_signal(&cond_timer);	
            }

            // unlock because if we entered the if statement then we must have acquired the lock
            // unlock so that timer can restart
            pthread_mutex_unlock(&mutex_timer);
		} 



		/*
		 *
		 *	CHECK FOR IO COMPLETION INTERRUPT FROM DEVICE A
		 *
		 */
		if(pthread_mutex_trylock(&mutex_io_a) == 0) {

			// check if timer a is done nanosleeping and if there's a process to be moved out of waitQ A
            if (io_timer_a_args->flag == 0 && !PCB_Queue_is_empty(waitingQueueA, &error)) {

                // call scheduler so that it moves PCB from appropriate waitQ to readyQ
                isrIO(INTERRUPT_TYPE_IO_A);

                // if the waitQ is not empty, then restart the io timer
                if (!PCB_Queue_is_empty(waitingQueueA, &error)) {
                    io_timer_a_args->flag = 1;
                    pthread_cond_signal(&cond_io_a);
                }
            }

			pthread_mutex_unlock(&mutex_io_a);
		} 

		/*
		 *
		 *	CHECK FOR IO COMPLETION INTERRUPT FROM DEVICE B
		 *
		 */
		if(pthread_mutex_trylock(&mutex_io_b) == 0) { 

			// check if timer a is done nanosleeping and if there's a process to be moved out of waitQ B
            if (io_timer_b_args->flag == 0 && !PCB_Queue_is_empty(waitingQueueB, &error)) {

                // call scheduler so that it moves PCB from appropriate waitQ to readyQ
                isrIO(INTERRUPT_TYPE_IO_B);

                // if the waitQ is not empty, then restart the io timer
                if (!PCB_Queue_is_empty(waitingQueueB, &error)) {
                    io_timer_b_args->flag = 1;
                    pthread_cond_signal(&cond_io_b);
                }
            }
			pthread_mutex_unlock(&mutex_io_b);
		}

		/*
		 *
		 *	CHECK IF WE'VE REACHED THE MAX PC
		 *
		 */
		if (sysStack >= currentPCB->max_pc) {
			sysStack = 0;
			currentPCB->term_count++;

			// if the process is one that is supposed to terminate (!= 0)
			// and if the process's terminate is 
			if (currentPCB->terminate != 0 && 
					currentPCB->terminate == currentPCB->term_count) {
				terminate();
			}
		} else {
			sysStack++;
		}

		if (currentPCB->type == IO_PROCESS) {
			for (i = 0; i < PCB_TRAP_LENGTH; i++) {
				if (sysStack == currentPCB->io_1_traps[i]) {
	                // enQ pcb into waitQ, get pcb from readyQ
					tsr(INTERRUPT_TYPE_IO_A);

	                if (ioThreadACreated == 0) {
	                    // this means it's the first IO trap for this IO timer
	                    ioThreadACreated = 1;
	                    if (pthread_create(&io_timer_a, NULL, timer, (void*) io_timer_a_args)) {
	                        printf("\nERROR creating io thread a");
	                        return 1;
	                    } else {
	                        printf("\nIO Timer A thread created\n");
	                    }
	                } else if (pthread_mutex_trylock(&mutex_io_a) == 0) {
	                    // if we're able to lock, means the IO device is stuck waiting on condition
						io_timer_a_args->flag = 1;
						pthread_cond_signal(&cond_io_a);
						pthread_mutex_unlock(&mutex_io_a);
					} 
                    break;
				} else if (sysStack == currentPCB->io_2_traps[i]) {
					tsr(INTERRUPT_TYPE_IO_B);
					
	                if (ioThreadBCreated == 0) {
	                    // this means it's the first IO trap for this IO timer
	                    ioThreadBCreated = 1;
	                    if(pthread_create(&io_timer_b, NULL, timer, (void*) io_timer_b_args)) {
	                        printf("\nERROR creating io thread b");
	                        return 1;
	                    } else {
	                        printf("\nIO Timer B thread created\n");
	                    }
	                } else if(pthread_mutex_trylock(&mutex_io_b) == 0) {
						io_timer_b_args->flag = 1;
						pthread_cond_signal(&cond_io_b);
						pthread_mutex_unlock(&mutex_io_b);
					}
                    break;
				}
			}
		} else if (currentPCB->type == PRODUCER_PROCESS || currentPCB->type == CONSUMER_PROCESS) {
            for (i = 0; i < PCB_TRAP_LENGTH; i++) {
				if (sysStack == currentPCB->lockR1[i]) {
                    int result;
                    printf("Attempt to LOCK R1 from %s Process 0x%lX, pair %d\n", (currentPCB->type == PRODUCER_PROCESS) ? "Producer" : "Consumer", currentPCB->pid, currentPCB->pair_id);
                    result = Mutex_lock(producerConsumerMutexes[currentPCB->pair_id], currentPCB, &error);
                    if (result == 0) {
                        printf("LOCKED R1\n");
                    } else {
                        printf("FAILED TO LOCK R1\n");
                        // mutex is a blocking construct, thus if we fail to lock, we get stuck
                        // put this PCB back into readyQ, make sure its pc isn't rolled past this trap
                        isrBlock();
                    }
                    break;
                } else if(sysStack == currentPCB->unlockR1[i]) {
                    int result;
                    printf("Attempt to UNLOCK R1 from %s Process 0x%lX, pair %d\n", (currentPCB->type == PRODUCER_PROCESS) ? "Producer" : "Consumer", currentPCB->pid, currentPCB->pair_id);
                    result = Mutex_unlock(producerConsumerMutexes[currentPCB->pair_id], currentPCB, readyQueue, &error);
                    if (result == 0) {
                        printf("UNLOCKED R1\n");
                    } else {
                        printf("FAILED TO UNLOCK R1\n");
                        // mutex is a blocking construct, thus if we fail to unlock, we get stuck
                        // put this PCB back into readyQ, make sure its pc isn't rolled past this trap
                        isrBlock();
                    }
                    break;
                } else if(sysStack == currentPCB->wait_condition[i]) {
                    int result;
                    printf("Attempt to WAIT for %s Process 0x%lX, pair %d\n", (currentPCB->type == PRODUCER_PROCESS) ? "Consumed CV from Producer" : "Produced CV from Consumer", currentPCB->pid, currentPCB->pair_id);
                    if (currentPCB->type == PRODUCER_PROCESS) {
                        result = Condition_wait(consumedCv[currentPCB->pair_id], producerConsumerMutexes[currentPCB->pair_id], readyQueue, &error);
                    } else {
                        result = Condition_wait(producedCv[currentPCB->pair_id], producerConsumerMutexes[currentPCB->pair_id], readyQueue, &error);
                    }
                    if (result == 0) {
                        printf("SUCCESS\n");
                        if (currentPCB->type == PRODUCER_PROCESS) {
                            printf("PRODUCE: %d becomes %d\n", producerConsumerVar[currentPCB->pair_id], producerConsumerVar[currentPCB->pair_id] + 1);
                            producerConsumerVar[currentPCB->pair_id]++;
                        } else {
                            printf("CONSUME: %d\n", producerConsumerVar[currentPCB->pair_id]);
                        }
                    } else {
                        printf("FAILURE\n");
                        // wait for condition is a blocking construct
                        // put this PCB back into readyQ, make sure its pc isn't rolled past this trap
                        isrBlock();
                    }
                    break;
                } else if(sysStack == currentPCB->signal_condition[i]) {
                    printf("Attempt to SIGNAL %s Process 0x%lX, pair %d\n", (currentPCB->type == PRODUCER_PROCESS) ? "Produced CV from Producer" : "Consumed CV from Consumer", currentPCB->pid, currentPCB->pair_id);
                    if (currentPCB->type == PRODUCER_PROCESS) {
                        Condition_signal(producedCv[currentPCB->pair_id], readyQueue, &error);
                    } else {
                        Condition_signal(consumedCv[currentPCB->pair_id], readyQueue, &error);
                    }
                    printf("SUCCESS\n");
                    break;
                }
            }
        } else if (currentPCB->type == MUTUAL_RESOURCE_PROCESS) {
            for (i = 0; i < PCB_TRAP_LENGTH; i++) {
                static int printed = 0;
				if (sysStack == currentPCB->lockR1[i]) {
                    int result;
                    printf("Attempt to LOCK R1 from Mutual Resource Process 0x%lX, pair %d\n", currentPCB->pid, currentPCB->pair_id);
                    result = Mutex_lock(MutualResourceMutexes[currentPCB->pair_id * 2], currentPCB, &error);
                    if (result == 0) {
                        printf("LOCKED R1\n");
                        if (!printed && 
                            MutualResourceMutexes[currentPCB->pair_id * 2]->is_locked == 1 && 
                            MutualResourceMutexes[currentPCB->pair_id * 2]->owner == currentPCB &&
                            MutualResourceMutexes[currentPCB->pair_id * 2 + 1]->is_locked == 1 &&
                            MutualResourceMutexes[currentPCB->pair_id * 2 + 1]->owner == currentPCB) {
                            printf("RESOURCE ACCESS\n");
                            printed = 1;
                        }
                    } else {
                        printf("FAILED TO LOCK R1\n");
#if (NO_DEADLOCKS == 0)
                        // mate must own this lock, check if we own the other lock
                        if (currentPCB->pid == MutualResourceMutexes[currentPCB->pair_id * 2 + 1]->owner->pid) {
                            printf("DEADLOCK DETECTED for processes PID: 0x%lX and PID: 0x%lX\n", currentPCB->pid, currentPCB->mate_pid);
                            // return 1;
                        } else {
                        	printf("NO DEADLOCK DETECTED for processes PID: 0x%lX and PID: 0x%lX\n", currentPCB->pid, currentPCB->mate_pid);
                        }
#endif
                        // mutex is a blocking construct, thus if we fail to lock, we get stuck
                        // put this PCB back into readyQ, make sure its pc isn't rolled past this trap
                        isrBlock();
                    }
                    break;
				} else if (sysStack == currentPCB->unlockR1[i]) {
                    int result;
                    printf("Attempt to UNLOCK R1 from Mutual Resource Process 0x%lX, pair %d\n", currentPCB->pid, currentPCB->pair_id);
                    // result = Mutex_unlock(MutualResourceMutexes[currentPCB->pair_id * 2], currentPCB, &error);
                    result = Mutex_unlock(MutualResourceMutexes[currentPCB->pair_id * 2], currentPCB, readyQueue, &error);

                    if (result == 0) {
                        printf("UNLOCKED R1\n");
                        printed = 0;
                    } else {
                        printf("FAILED TO UNLOCK R1\n");
                        // mutex is a blocking construct, thus if we fail to unlock, we get stuck
                        // put this PCB back into readyQ, make sure its pc isn't rolled past this trap
                        isrBlock();
                    }
                    break;
				} else if (sysStack == currentPCB->lockR2[i]) {
                    int result;
                    printf("Attempt to LOCK R2 from Mutual Resource Process 0x%lX, pair %d\n", currentPCB->pid, currentPCB->pair_id);
                    result = Mutex_lock(MutualResourceMutexes[currentPCB->pair_id * 2 + 1], currentPCB, &error);
                    if (result == 0) {
                        printf("LOCKED R2\n");
                        if (!printed && 
                            MutualResourceMutexes[currentPCB->pair_id * 2]->is_locked == 1 && 
                            MutualResourceMutexes[currentPCB->pair_id * 2]->owner == currentPCB &&
                            MutualResourceMutexes[currentPCB->pair_id * 2 + 1]->is_locked == 1 &&
                            MutualResourceMutexes[currentPCB->pair_id * 2 + 1]->owner == currentPCB) {
                            printf("RESOURCE ACCESS\n");
                            printed = 1;
                        }
                    } else {
                        printf("FAILED TO LOCK R2\n");
#if (NO_DEADLOCKS == 0)
                        // mate must own this lock, check if we own the other lock
                        if (currentPCB->pid == MutualResourceMutexes[currentPCB->pair_id * 2]->owner->pid) {
                            printf("DEADLOCK DETECTED for processes PID: 0x%lX and PID: 0x%lX\n", currentPCB->pid, currentPCB->mate_pid);
                          	// return 1;
                        } else {
                        	printf("NO DEADLOCK DETECTED for processes PID: 0x%lX and PID: 0x%lX\n", currentPCB->pid, currentPCB->mate_pid);
                        }
#endif
                        // mutex is a blocking construct, thus if we fail to lock, we get stuck
                        // put this PCB back into readyQ, make sure its pc isn't rolled past this trap
                        isrBlock();
                    }
                    break;
				} else if (sysStack == currentPCB->unlockR2[i]) {
                    int result;
                    printf("Attempt to UNLOCK R2 from Mutual Resource Process 0x%lX, pair %d\n", currentPCB->pid, currentPCB->pair_id);
                    //result = Mutex_unlock(MutualResourceMutexes[currentPCB->pair_id * 2 + 1], currentPCB, &error);
                    result = Mutex_unlock(MutualResourceMutexes[currentPCB->pair_id * 2 + 1], currentPCB, readyQueue, &error);
                    if (result == 0) {
                        printf("UNLOCKED R2\n");
                        printed = 0;
                    } else {
                        printf("FAILED TO UNLOCK R2\n");
                        // mutex is a blocking construct, thus if we fail to unlock, we get stuck
                        // put this PCB back into readyQ, make sure its pc isn't rolled past this trap
                        isrBlock();
                    }
                    break;
                }
			}
		}

		incrementStarvationQuanta(readyQueue, &error);

	}
    
    printf("\n");
	printf("SIMULATION COMPLETE\n");

#if (NO_DEADLOCKS == 1)
		printf("\nNO DEADLOCKs DETECTED\n");
#endif

    printf("\nReady Queue after completion: Size = %d\n", readyQueue->size);
    PCB_Priority_Queue_print(readyQueue, &error);

    printf("\nWaiting Queue A after completion:\n");
    PCB_Queue_print(waitingQueueA, &error);

    printf("\nWaiting Queue B after completion:\n");
    PCB_Queue_print(waitingQueueB, &error);

    printf("\nTotal Processes run: %d\n", total_processes_run_count);

    destroy_everything(&error); //Clean up memory resources

    return 0;
}