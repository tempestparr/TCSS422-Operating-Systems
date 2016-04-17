/*
 * Problem 1 - OS Abstract Data Types
 * TCSS 422 A Spring 2016
 * Christopher Ottersen, Tempest Parr, Mark Peters
 */

#ifndef PCB_H
#define PCB_H
#define NUMREGS 16
#define new new_
#define bool bool_
#define true true_
#define false false_

#define PRIORITIES_TOTAL 16
#define LOWEST_PRIORITY (PRIORITIES_TOTAL - 1)

typedef enum {false, true} bool;

enum state_type {new, ready, running, interrupted, waiting, halted};

typedef struct pcb PCB;
typedef PCB * PCB_p;

PCB_p 			PCB_construct 	(int *ptr_error);     
int 			PCB_destruct 	(PCB_p this); 
int 			PCB_init 		(PCB_p this);      
int 			PCB_setPid 		(PCB_p this, unsigned long pid); 
unsigned long 	PCB_getPid 		(PCB_p this, int *ptr_error);
int 			PCB_setState 	(PCB_p this, enum state_type state); 
enum state_type PCB_getState 	(PCB_p this, int *ptr_error);
int 			PCB_setPriority (PCB_p this, unsigned short priority); 
unsigned short 	PCB_getPriority (PCB_p this, int *ptr_error);
int 			PCB_setPc 		(PCB_p this, unsigned long pc); 
unsigned long 	PCB_getPc 		(PCB_p this, int *ptr_error);
char* 			PCB_toString 	(PCB_p this, char *str, int *ptr_error);
int 			PCB_compareTo	(PCB_p this, PCB_p other, int *ptr_error);

int 			PCB_test_main	(int argc, char** argv);

#endif

