/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   pcb.h
 * Author: chris
 *
 * Created on March 25, 2016, 5:20 PM
 */

#ifndef PCB_H
#define PCB_H

#include <stdio.h>
// all #defines
#define NUMREGS 16
// to shut netbeans up
#define new new_
#define bool bool_
#define true true_
#define false false_

#define PRIORITIES_TOTAL 32
#define LOWEST_PRIORITY (PRIORITIES_TOTAL - 1)

typedef enum {false, true} bool;
//#define this this_
// all typedefs

enum state_type {new, ready, running, interrupted, waiting, halted};

struct pcb {
  unsigned long pid;        // process ID #, a unique number
  enum state_type state;    // process state (running, waiting, etc.)
  unsigned short priority;  // priorities 0=highest, LOWEST_PRIORITY=lowest
  unsigned long pc;         // holds the current pc value when pre emptied
};

typedef struct pcb PCB;
typedef PCB * PCB_p;

// Member Functions
/**
 * Returns a pcb pointer to heap allocation.
 * @return a pointer to a new PCB object
 */
PCB_p PCB_construct (int *ptr_error);     

/**
 * Deallocates pcb from the heap.
 * @param this
 */
int PCB_destruct (PCB_p this); 

/**
 * Sets default values for member data.
 * @param this
 * @return 
 */
int PCB_init (PCB_p this);      

/**
 * Sets the pid of the process.
 * 
 * @param this 
 * @param pid the new pid value
 * @return 
 */
int PCB_setPid (PCB_p this, unsigned long pid); 

/**
 * Returns the pid of the process.
 * 
 * @param this
 * @return the pid of the process
 */
unsigned long PCB_getPid (PCB_p this, int *ptr_error);

/**
 * Sets the pid of the process.
 * 
 * @param this 
 * @param pid the new pid value
 * @return 
 */
int PCB_setState (PCB_p this, enum state_type state); 

/**
 * Returns the current state of the process.
 * 
 * @param this
 * @return the pid of the process
 */
enum state_type PCB_getState (PCB_p this, int *ptr_error);


/**
 * Sets the pid of the process.
 * 
 * @param this 
 * @param pid the new pid value
 * @return 
 */
int PCB_setPriority (PCB_p this, unsigned short priority); 

/**
 * Returns the current state of the process.
 * 
 * @param this
 * @return the pid of the process
 */
unsigned short PCB_getPriority (PCB_p this, int *ptr_error);

/**
 * Sets the pid of the process.
 * 
 * @param this 
 * @param pid the new pid value
 * @return 
 */
int PCB_setPc (PCB_p this, unsigned long pc); 

/**
 * Returns the pc value state of the process.
 * 
 * @param this
 * @return the address where the program will resume
 */
unsigned long PCB_getPc (PCB_p this, int *ptr_error);

/**
 * Returns a string representing the contents of the pcb. 
 * <br /><em><strong>Note:</strong> returned string must be freed.</em>
 * @param this
 * @return string representing the contents of the pcb
 */
char * PCB_toString (PCB_p this, char *str, int *ptr_error);

#endif /* PCB_H */

