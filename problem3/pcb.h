/*
 *  TCSS 422 Spring 2016
 *  Team 4 Luis Solis-Bruno, Mat Sharff, Tempest Parr, Sara Vandandaigue
 *  Problem 3 Discontinuities
 */

#ifndef PCB_H
#define PCB_H

/* Default values */
#define DEFAULT_PID 0
#define DEFAULT_STATE new
#define DEFAULT_PC 0
#define DEFAULT_PRIORITY 0
#define DEFAULT_SW 0
#define DEFAULT_MAX_PC 0
#define DEFAULT_CREATION NULL 
#define DEFAULT_TERMINATION NULL
#define DEFAULT_TERMINATE 0
#define DEFAULT_TERM_COUNT 0

#define ALLOW_ENDLESS_PROCESSES 0
 
/* Error Handling Values */
#define SUCCESS 0
#define NULL_OBJECT -1

/* typedefs for State, PCB, and PCB_p */
typedef enum state_type {
  new, ready, running, interrupted, waiting, halted
} State;

typedef struct pcb {
  unsigned long pid;
  State state;
  unsigned short priority;
  unsigned long pc;
  unsigned long sw;
  unsigned long max_pc;
  unsigned long creation;
  unsigned long termination;
  unsigned long terminate;
  unsigned long term_count;
  unsigned long IO_1_trap[4];
  unsigned long IO_2_trap[4]; 
} PCB;

typedef PCB * PCB_p;

/* Function Prototypes */
PCB_p PCB_construct(void);
void PCB_destruct(PCB_p pcb);
int PCB_init(PCB_p pcb);
int PCB_set_pid(PCB_p pcb, unsigned long pid);
unsigned long PCB_get_pid(PCB_p pcb);
int PCB_set_state(PCB_p pcb, State state);
State PCB_get_state(PCB_p pcb);
int PCB_set_priority(PCB_p pcb, unsigned short priority);
unsigned short PCB_get_priority(PCB_p pcb);
char * PCB_toString(PCB_p pcb, char * string);

#endif
