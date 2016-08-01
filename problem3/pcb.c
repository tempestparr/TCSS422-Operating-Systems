/*
 *  TCSS 422 Spring 2016
 *  Team 4 Luis Solis-Bruno, Mat Sharff, Tempest Parr, Sara Vandandaigue
 *  Problem 3 Discontinuities
 */

#include <stdlib.h>
#include "pcb.h"

const char * STATE_NAMES[] = {"new", "ready", "running", "interrupted", "waiting", "halted"};

PCB_p PCB_construct (void) {
    return malloc(sizeof(PCB));
}

void PCB_destruct(PCB_p pcb) {
    if(pcb != NULL) free(pcb);
}

int PCB_init(PCB_p pcb) {
    int i = 0;

    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->pid = DEFAULT_PID;
    pcb->state = DEFAULT_STATE;
    pcb->priority = DEFAULT_PRIORITY;
    pcb->pc = DEFAULT_PC;
    pcb->sw = DEFAULT_SW;
    pcb->max_pc = rand() % 6000 + 2000;
    pcb->creation = time(NULL);
    pcb->termination = (long) DEFAULT_TERMINATION;
#if (ALLOW_ENDLESS_PROCESSES == 1)
    pcb->terminate = rand() % 16;
#else 
    pcb->terminate = 1 + (rand() % 15);
#endif
    pcb->term_count = DEFAULT_TERM_COUNT;
    //printf("pid %d has maxpc %ld\n", pcb->pid, pcb->max_pc);
    for(i = 0; i < 8; i++ ){
        if (i % 2 == 0) {
            pcb->IO_1_trap[i / 2] = (pcb->max_pc / 8) * i + rand() % (pcb->max_pc / 8);
      //      printf("pid %d has io_1_trap[%d] %ld\n", pcb->pid, i / 2, pcb->IO_1_trap[i / 2]);
        } else {
            pcb->IO_2_trap[(i - 1) / 2] = (pcb->max_pc / 8) * i + rand() % (pcb->max_pc / 8);
      //      printf("pid %d has io_2_trap[%d] %ld\n", pcb->pid, (i - 1) / 2, pcb->IO_2_trap[(i - 1) / 2]);
        }
    }

    return SUCCESS;
}

int PCB_set_pid(PCB_p pcb, unsigned long pid) {
    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->pid = pid;
    return SUCCESS;
}

unsigned long PCB_get_pid(PCB_p pcb) {
    //error checking in controller
    return pcb->pid;
}

int PCB_set_state(PCB_p pcb, State state) {
    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->state = state;
    return SUCCESS;
}

State PCB_get_state(PCB_p pcb) {
    return pcb->state;
}

int PCB_set_priority(PCB_p pcb, unsigned short priority) {
    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->priority = priority;
    return SUCCESS;
}

unsigned short PCB_get_priority(PCB_p pcb) {
    return pcb->priority;
}


char * PCB_toString(PCB_p pcb, char * string) {
    sprintf(string, "PID: 0x%0lX, State: %s, Priority: 0x%0X, PC: 0x%04lX, SW: 0x%04lX, max_pc: 0x%04lX, creation: 0x%lX, termination: 0x%lX, terminate: 0x%lX, term_count: %ld",
            pcb->pid, STATE_NAMES[pcb->state], pcb->priority, pcb->pc, pcb->sw, pcb->max_pc, pcb->creation, pcb->termination, pcb->terminate, pcb->term_count);
    return string;
}
