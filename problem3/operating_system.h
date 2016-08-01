/*
 *  TCSS 422 Spring 2016
 *  Team 4 Luis Solis-Bruno, Mat Sharff, Tempest Parr, Sara Vandandaigue
 *  Problem 3 Discontinuities
 */

#include "pcb.c"
#include "fifo_queue.c"

enum interrupt_type {
  IO_1, IO_2, TIMER, NEW_PROC_NEEDED, INVALID_INT_TYPE
};

#define INTERRUPTED 1
#define NEW_PROCESSES 2

//
// Defines how many times the create_processses function is called
//
#define CREATE_PROCESSES_CALL_COUNT 5

void createProcesses(void);
void schedulePCBs(int);
void dispatch(void);
void interruptSR(void);
void setup(void);
