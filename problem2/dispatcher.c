/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#include <stdio.h>

#include "dispatcher.h"
#include "FIFOq.h"
#include "PCB.h"
#include "CPU.h"

void dispatcher(FIFOq_p readyQ, PCB_p current, int* error)
{
	//save PC value into PCB
	PCB_setPc (current, current->pc);

	//dequeue the head of readyQueue
	FIFOq_dequeue(readyQ, error);

	//change PCB's state to running point
	PCB_setState (current, running);

	//copy PCB's PC value to SystemStack
	SysStack[++SysPointer] = current->pc;
}
