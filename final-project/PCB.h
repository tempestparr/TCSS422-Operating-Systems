/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 *
 */

//#include "mutex.h"
 
#pragma once

#define PCB_PRIORITY_MAX 3
#define PCB_TRAP_LENGTH 4
#define MAX_PC 5000000
#define ALLOW_ENDLESS_PROCESSES 0

 enum PCB_ERROR {
	PCB_SUCCESS = 0,
	PCB_NULL_POINTER,
	PCB_INVALID_ARG,
	PCB_MEM_ALLOC_FAIL,

	PCB_LAST_ERROR
};

enum PCB_STATE_TYPE {
	PCB_STATE_NEW = 0, 
	PCB_STATE_READY,        // 1
	PCB_STATE_RUNNING,      // 2
	PCB_STATE_INTERRUPTED,  // 3
	PCB_STATE_WAITING,      // 4
	PCB_STATE_HALTED,       // 5

	PCB_STATE_LAST_ERROR, 	// invalid type, used for bounds checking
	PCB_STATE_ERROR 		// Used as return value if null pointer is passed to getter.
};

enum PCB_PROCESS_TYPE {
	COMPUTE_INTENSIVE_PROCESS,
	IO_PROCESS,
	PRODUCER_PROCESS,
	CONSUMER_PROCESS,
	MUTUAL_RESOURCE_PROCESS,
	UNDEFINED_PROCESS
};

struct PCB {
    unsigned long pid;        		// process ID #, a unique number
	unsigned short priority;  		// priorities 0=highest, 15=lowest
	enum PCB_STATE_TYPE state;    	// process state (running, waiting, etc.)
	unsigned long pc;         		// holds the current pc value when preempted
	unsigned int sw;
	unsigned long max_pc;			// number of instructions that should be processed before resetting PC to 0
	unsigned long creation;			// computer clock time when the process was created
	unsigned long termination;		// computer clock time when the process terminates
	unsigned int terminate;			// number of times the PC will pass its MAX_PC value, 0 for no terminate
	unsigned int term_count;		// counter to track of how many times the process has passed its MAX_PC value
	unsigned int has_been_run;

	unsigned long io_1_traps[PCB_TRAP_LENGTH];
	unsigned long io_2_traps[PCB_TRAP_LENGTH];

	unsigned int priority_boost; 			// starvation prevention flag to boost priority of PCB
	unsigned int starvation_quanta_count; 	// count of the quanta for starvation prevention of PCB
	
	enum PCB_PROCESS_TYPE type;				// type of process a PCB is

	unsigned int pair_id;					// name for pairs, if PCB type is producer/consumer
	unsigned long mate_pid;

	// struct mutex_type *mutex_1;
	// struct mutex_type *mutex_2;
	// struct condition_var_type *condition_var_1;
	// struct condition_var_type *condition_var_2;

	unsigned long lockR1[PCB_TRAP_LENGTH];				// These are the arrays that determine when the process
    unsigned long lockR2[PCB_TRAP_LENGTH];              // Will perform the corresponding behavior.
	// unsigned long trylock[PCB_TRAP_LENGTH];		    // not used
	unsigned long unlockR1[PCB_TRAP_LENGTH];
    unsigned long unlockR2[PCB_TRAP_LENGTH];
	unsigned long signal_condition[PCB_TRAP_LENGTH];
	unsigned long wait_condition[PCB_TRAP_LENGTH];

};

typedef struct PCB * PCB_p;

PCB_p PCB_construct(enum PCB_ERROR*); 		// returns a pcb pointer to heap allocation
void PCB_destruct(PCB_p, enum PCB_ERROR*);  // deallocates pcb from the heap
void PCB_init(PCB_p, enum PCB_ERROR*);      // sets default values for member data

void PCB_set_pid(PCB_p, unsigned long, enum PCB_ERROR*);
void PCB_set_priority(PCB_p, unsigned short, enum PCB_ERROR*);
void PCB_set_state(PCB_p, enum PCB_STATE_TYPE, enum PCB_ERROR*);
void PCB_set_pc(PCB_p, unsigned long, enum PCB_ERROR*);
void PCB_set_sw(PCB_p, unsigned int, enum PCB_ERROR*);
void PCB_set_max_pc(PCB_p, unsigned long, enum PCB_ERROR*);
void PCB_set_creation(PCB_p, unsigned long, enum PCB_ERROR*);
void PCB_set_termination(PCB_p, unsigned long, enum PCB_ERROR*);
void PCB_set_terminate(PCB_p, unsigned int, enum PCB_ERROR*);
void PCB_set_term_count(PCB_p, unsigned int, enum PCB_ERROR*);

unsigned long PCB_get_pid(PCB_p, enum PCB_ERROR*);  // returns pid of the process
unsigned short PCB_get_priority(PCB_p, enum PCB_ERROR*);
enum PCB_STATE_TYPE PCB_get_state(PCB_p, enum PCB_ERROR*);
unsigned long PCB_get_pc(PCB_p, enum PCB_ERROR*);
unsigned int PCB_get_sw(PCB_p, enum PCB_ERROR*);
unsigned long PCB_get_max_pc(PCB_p, enum PCB_ERROR*);
unsigned long PCB_get_creation(PCB_p, enum PCB_ERROR*);
unsigned long PCB_get_termination(PCB_p, enum PCB_ERROR*);
unsigned int PCB_get_terminate(PCB_p, enum PCB_ERROR*);
unsigned int PCB_get_term_count(PCB_p, enum PCB_ERROR*);

void PCB_print(PCB_p, enum PCB_ERROR*);  // a string representing the contents of the pcb
const char *PCB_type_toString(enum PCB_PROCESS_TYPE);
const char *PCB_state_toString(enum PCB_STATE_TYPE);
