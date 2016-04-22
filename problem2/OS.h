/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#ifndef OS_H
#define OS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "FIFOq.h"
#include "PCB.h"

#define DEBUG true
#define EXIT_STATUS_MESSAGE true
#define NO_INTERRUPT 9999
#define INTERRUPT_CREATE 6666
#define INTERRUPT_TIMER 5555
#define INTERRUT_IO 4444

#define MAX_PROCESSES 30
#define SYSSIZE 256
#define MAX_NEW_PCB 5
#define RUN_MIN_TIME 3000
#define RUN_TIME_RANGE 1000

#define CPU_NULL_ERROR 71

extern unsigned int SysStack[SYSSIZE];
extern int SysPointer;

void    startCPU    ();
int     mainLoopOS  (int *error);
void    queueCleanup(FIFOq_p, char*, int*);
void    stackCleanup();
int    createPCBs  	(FIFOq_p createQ, int *error);
void    run         (unsigned int *pc, int *error);

#endif