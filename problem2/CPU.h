/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "FIFOq.h"

#define NO_INTERRUPT 9999
#define INTERRUPT_CREATE 6666
#define INTERRUPT_TIMER 5555
#define INTERRUT_IO 4444

#define SYSSIZE 256

extern unsigned int SysStack[SYSSIZE];
extern int SysPointer;

void startCPU();
int mainLoopOS(int *error);
void queueCleanup(FIFOq_p, char*, int*);
void stackCleanup();

#endif
