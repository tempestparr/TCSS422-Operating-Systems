/*
 * Problem 1 - OS Abstract Data Types
 * TCSS 422 A Spring 2016
 * Christopher Ottersen, Tempest Parr, Mark Peters
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "pcb.h"

#define DEFAULT_STATE new
#define DEFAULT_PC 0Lu
#define PCB_NULL_ERROR 5
#define PCB_INIT_ERROR 7
#define PCB_OTHER_ERROR 41


struct pcb {
  unsigned long pid;        // process ID #, a unique number
  enum state_type state;    // process state (running, waiting, etc.)
  unsigned short priority;  // priorities 0=highest, LOWEST_PRIORITY=lowest
  unsigned long pc;         // holds the current pc value when pre emptied
};

/**
 * Returns a pcb pointer to heap allocation.
 * 
 * @return a pointer to a new PCB object
 */
PCB_p PCB_construct (int *ptr_error) {
  PCB_p this = (PCB_p) malloc(sizeof(PCB));
  
  int error = ((!this) * PCB_INIT_ERROR);
  this->pc = 0;
  this->pid = 0;
  this->priority = 0;
  this->state = 0;

  if(ptr_error != NULL) {
    *ptr_error += error;
  }
  return (this);
}

/**
 * Deallocates pcb from the heap.
 * @param this
 */
int PCB_destruct (PCB_p this) {
  int error = (this == NULL) * PCB_NULL_ERROR; // sets error code to 1 if `this` is NULL
  if(!error) {
    free(this);
    this = NULL;
  }
  return error;
}

/**
 * Sets default values for member data.
 * @param this
 * @return 
 */
int PCB_init (PCB_p this) {
  static unsigned long pidCounter = 0;
  static int firstCall = 1;
  if (!firstCall) {
    srand(time(NULL)<<1);
    firstCall = 0;
  }
  int error = (this == NULL) * PCB_NULL_ERROR;
  
  if(!error) {
    this->pid = ++pidCounter;
    this->pc = DEFAULT_PC;
    this->priority = rand() & LOWEST_PRIORITY;
    this->state = DEFAULT_STATE;
  }
  return error;
}

/**
 * Sets the pid of the process.
 * 
 * @param this 
 * @param pid the new pid value
 * @return 
 */
int PCB_setPid (PCB_p this, unsigned long pid) {
  // verify pid does not already exist
  int error = (this == NULL) * PCB_NULL_ERROR;
  if(!error) {
    this->pid = pid;
  }
  return error;// TODO: write
}

/**
 * Returns the pid of the process.
 * 
 * @param this
 * @return the pid of the process
 */
unsigned long PCB_getPid (PCB_p this, int *ptr_error) {
  int error = (this == NULL) * PCB_NULL_ERROR;
  
  if (ptr_error != NULL) {
    *ptr_error += error;
  }
  
  return error ? ~0 : this->pid;// TODO: write
}

/**
 * Sets the state of the process.
 * 
 * @param this 
 * @param state the new state value
 * @return error 
 */
int PCB_setState (PCB_p this, enum state_type state) {
  int error = 0;
  if(this == NULL) {
    error |= PCB_NULL_ERROR;
  }
  if(state < new || state > halted) {
    error |= PCB_OTHER_ERROR;
  }
  if(!error) {
    this->state = state;
  }
  return error;
}

/**
 * Returns the current state of the process.
 * 
 * @param this
 * @return the state of the process
 */
enum state_type PCB_getState (PCB_p this, int *ptr_error) {
  int error = (this == NULL) * PCB_NULL_ERROR;
  
  if(ptr_error != NULL) {
    *ptr_error += error;
  }
  return error ? ~0 : this->state;
}


/**
 * Sets the pid of the process.
 * 
 * @param this 
 * @param pid the new pid value
 * @return 
 */
int PCB_setPriority (PCB_p this, unsigned short priority) {
  int error = 0;
  if(this == NULL) {
    error |= PCB_NULL_ERROR;
  }
  if(priority > LOWEST_PRIORITY) {
    error |= PCB_OTHER_ERROR;
  }
  if (!error) {
    this->priority = priority; 
  }
  return error;
}

/**
 * Returns the current state of the process.
 * 
 * @param this
 * @return the pid of the process
 */
unsigned short PCB_getPriority (PCB_p this, int *ptr_error) {
  int error = (this == NULL) * PCB_NULL_ERROR;
  
  if(ptr_error != NULL) {
    *ptr_error += error;
  }
  return error ? ~0 : this->priority; // TODO: write
}

/**
 * Sets the pid of the process.
 * 
 * @param this 
 * @param pid the new pid value
 * @return 
 */
int PCB_setPc (PCB_p this, unsigned long pc) {
  int error = (this == NULL) * PCB_NULL_ERROR;
  if (!error) {
    this->pc = pc;
  }
  return error; // TODO: write
}

/**
 * Returns the pc value state of the process.
 * 
 * @param this
 * @return the address where the program will resume
 */
unsigned long PCB_getPc (PCB_p this, int *ptr_error) {
  int error = (this == NULL) * PCB_NULL_ERROR;
  if(ptr_error != NULL) {
    *ptr_error += error;
  }
  return error ? ~0 : this->pc; // TODO: write
}
/**
 * Returns a string representing the contents of the pcb. 
 * <br /><em><strong>Note:</strong> parameter string must be 80 chars or more.</em>
 * @param this
 * @param str
 * @param int
 * @return string representing the contents of the pc.
 */
char * PCB_toString (PCB_p this, char *str, int *ptr_error) {
  int error = (this == NULL || str == NULL) * PCB_NULL_ERROR;
  if(!error) {
    str[0] = '\0';
    const char * format = "PID: 0x%x, Priority 0x%x, state: %d, PC: 0x%04x";
    snprintf (str, (size_t) 79, format, this->pid, this->priority, this->state, this->pc);
  }
  
  if(ptr_error != NULL) {
    *ptr_error += error;
  }
  return str;
  
}