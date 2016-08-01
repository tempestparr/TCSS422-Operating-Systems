/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 *
 */

#include "PCB.h"
#include "mutex.h"
#include "cond_var.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

PCB_p PCB_construct(enum PCB_ERROR *error) {
	int i = 0;
	PCB_p p = malloc(sizeof(struct PCB));
    
	if (p == NULL) {
		*error = PCB_MEM_ALLOC_FAIL;
		return NULL;
	}

	p->pid = 0;
	p->state = PCB_STATE_NEW;
	p->priority = PCB_PRIORITY_MAX;
	p->pc = 0;
	p->max_pc = rand() % 6000 + 2000;
	p->has_been_run = 0;

	// p->terminate = 1;
#if (ALLOW_ENDLESS_PROCESSES == 1)
    p->terminate = rand() % 16;
#else 
    p->terminate = 1 + (rand() % 15);
#endif

	p->creation = time(NULL);
	p->termination = 0;

	for(i = 0; i < 8; i++ ){
        if (i % 2 == 0) {
            p->io_1_traps[i / 2] = (p->max_pc / 8) * i + rand() % (p->max_pc / 8);
        } else {
            p->io_2_traps[(i - 1) / 2] = (p->max_pc / 8) * i + rand() % (p->max_pc / 8);
        }
    }

	p->type = UNDEFINED_PROCESS;
	p->priority_boost = 0;
	p->starvation_quanta_count = 0;

	return p;
}

void PCB_destruct(PCB_p p, enum PCB_ERROR *error) {
	free(p);
}

void PCB_init(PCB_p p, enum PCB_ERROR *error) {
}

void PCB_set_type(PCB_p p, enum PCB_PROCESS_TYPE t, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->type = t;
}

void PCB_set_pid(PCB_p p, unsigned long pid, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->pid = pid;
}

void PCB_set_state(PCB_p p, enum PCB_STATE_TYPE state, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	if (state >= PCB_STATE_LAST_ERROR) {
		*error = PCB_INVALID_ARG;
		return;
	}
	p->state = state;
}

void PCB_set_priority(PCB_p p, unsigned short priority, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	if (priority > PCB_PRIORITY_MAX) {
		*error = PCB_INVALID_ARG;
		return;
	}
	p->priority = priority;
}

void PCB_set_pc(PCB_p p, unsigned long pc, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->pc = pc;
}

void PCB_set_sw(PCB_p p, unsigned int i, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->sw = i;
}
void PCB_set_max_pc(PCB_p p, unsigned long i, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->max_pc = i;
}
void PCB_set_creation(PCB_p p, unsigned long i, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->creation = i;
}
void PCB_set_termination(PCB_p p, unsigned long i, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->termination = i;
}
void PCB_set_terminate(PCB_p p, unsigned int i, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->terminate = i;
}
void PCB_set_term_count(PCB_p p, unsigned int i, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	p->term_count = i;
}

unsigned long PCB_get_pid(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}
	return p->pid;
}  

enum PCB_STATE_TYPE PCB_get_state(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return PCB_STATE_ERROR;
	}
	return p->state;
}

unsigned short PCB_get_priority(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}
	return p->priority;
}

unsigned long PCB_get_pc(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}
	return p->pc;
} 

unsigned int PCB_get_sw(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}
	return p->sw;
}
unsigned long PCB_get_max_pc(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}	
	return p->max_pc;
}

unsigned long PCB_get_creation(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}	
	return p->creation;
}

unsigned long PCB_get_termination(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}	
	return p->termination;
}

unsigned int PCB_get_terminate(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}
	return p->terminate;
}

unsigned int PCB_get_term_count(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return 0;
	}
	return p->term_count;
}

unsigned short PCB_get_type(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return UNDEFINED_PROCESS;
	}
	return p->type;
}

void PCB_print(PCB_p p, enum PCB_ERROR *error) {
	if (p == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	printf("PID: 0x%lX, Type: %s, Priority: 0x%X, State: %s, PC: 0x%lX, MaxPC: 0x%lx, Terminate: %u, TermCount: %u\n", 
			PCB_get_pid(p, error), PCB_type_toString(PCB_get_type(p, error)), PCB_get_priority(p, error),
			PCB_state_toString(PCB_get_state(p, error)), PCB_get_pc(p, error),
			PCB_get_max_pc(p, error), PCB_get_terminate(p, error),
			PCB_get_term_count(p, error));
            
    // if (p->type == MUTUAL_RESOURCE_PROCESS) {
        // printf("R1_lock_traps: 0x%lx, 0x%lx, 0x%lx, 0x%lx\n",   p->lockR1[0], p->lockR1[1], p->lockR1[2], p->lockR1[3]);
        // printf("R1_unlock_traps: 0x%lx, 0x%lx, 0x%lx, 0x%lx\n", p->unlockR1[0], p->unlockR1[1], p->unlockR1[2], p->unlockR1[3]);
        // printf("R2_lock_traps: 0x%lx, 0x%lx, 0x%lx, 0x%lx\n",   p->lockR2[0], p->lockR2[1], p->lockR2[2], p->lockR2[3]);
        // printf("R2_unlock_traps: 0x%lx, 0x%lx, 0x%lx, 0x%lx\n", p->unlockR2[0], p->unlockR2[1], p->unlockR2[2], p->unlockR2[3]);
    // }
    // printf("IO_1_traps: 0x%lx, 0x%lx, 0x%lx, 0x%lx\n", p->io_1_traps[0], p->io_1_traps[1], p->io_1_traps[2], p->io_1_traps[3]);
    // printf("IO_2_traps: 0x%lx, 0x%lx, 0x%lx, 0x%lx\n", p->io_2_traps[0], p->io_2_traps[1], p->io_2_traps[2], p->io_2_traps[3]);
    // todo: print out termination, creation,  io traps
}

/**
 * Returns the easy-to-read name of the process type. 
 */
const char *PCB_type_toString(enum PCB_PROCESS_TYPE t) {
	switch (t) {
		case COMPUTE_INTENSIVE_PROCESS:
			return "INTENSIVE";
		case IO_PROCESS:
			return "I/O";
		case PRODUCER_PROCESS:
			return "PRODUCER";
		case CONSUMER_PROCESS:
			return "CONSUMER";
		case MUTUAL_RESOURCE_PROCESS:
			return "MUTUAL RESOURCE";
		default:
			return "UNDEFINED";
	}
}

/**
 * Returns the easy-to-read name of the process state.
 */
const char *PCB_state_toString(enum PCB_STATE_TYPE t) {
	switch (t) {
		case PCB_STATE_NEW:
			return "NEW";
		case PCB_STATE_READY:
			return "READY";
		case PCB_STATE_RUNNING:
			return "RUNNING";
		case PCB_STATE_INTERRUPTED:
			return "INTERRUPTED";
		case PCB_STATE_WAITING:
			return "WAITING";
		case PCB_STATE_HALTED:
			return "HALTED";
		case PCB_STATE_LAST_ERROR:
			return "LAST_ERROR";
		default:
			return "ERROR";
	}
}

/* PRODUCER CONSUMER FUNCTIONS */
// void PCB_produce(PCB_p pcb, Mutex_p m) {
// 	// Mutex_lock(m, pcb);
// }

// void PCB_consume(/* MUTEX */) {

// }
