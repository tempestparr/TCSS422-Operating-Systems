/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#include <stdio.h>

#include "Interrupt.h"
#include "FIFOq.h"
#include "PCB.h"
#include "CPU.h"
#include "dispatcher.h"

//timer interrupt
void isrTimer(int INTERRUPT, FIFOq_p createQ, FIFOq_p readyQ, PCB current, int* error)
{
	//variables for big PC and big SW
	unsigned long bigPC = 0;
	unsigned long bigSW = 0;

	//change the state from running to interrupted
	PCB_setState (current, interrupted);

	//pop off SystemStack
	//create temporary PCB
	PCB_p tempHolder = PCB_construct(error);

	//pop off node on SystemStack and make temporary PCB it
	tempHolder = FIFOq_dequeue(SysStack, error);

	//assigns Current PCB PC and SW values to popped values of SystemStack
	SysStack[++SysPointer] = current->pc;
	SysStack[++SysPointer] = current->sw;

	//delete this PCB
	free(tempHolder);

	//call Scheduler and pass timer interrupt parameter
	scheduler(INTERRUPT, createQ, readyQ, current, * error);

	//Scheduler sets Current PCB state to ready
	//PCB_setState (current, ready);
	//found in scheduler function of CPU.c

	//move it back to ReadyQueue
	//found in scheduler function of CPU.c
	//while the queue is not empty
	/*while (createQ->size != 0)
	{
		//create temporary PCB
		PCB_p temp = PCB_construct(error);

		//make temporary PCB the recently dequeued node
		temp = FIFOq_dequeue(createQ, error);

		//change PCB state to ready
		temp->state = ready;

		//copy temporary PCB into queue
		FIFOq_enqueuePCB(readyQ, temp, error);

		//delete temporary PCB
		free(temp);
	}*/


	//make Current pointer to IDLE or NULL
	//PCB_setState (current, halted);

	//Scheduler calls dispatcher
	dispatcher(readyQ, current, * error);

	//take PCB's PC and SW values and push them into SystemStack
	SysStack[++SysPointer] = current->pc;
	SysStack[++SysPointer] = current->sw;

	//Scheduler does housekeeping
	//does nothing

	//Interrupt pops SystemStack and assign SW and PC values to bigSW and bigPC
	//
}
