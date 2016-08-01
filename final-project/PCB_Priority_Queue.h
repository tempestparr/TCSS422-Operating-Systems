/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 *
 */

#pragma once

#include "PCB_Queue.h"
#include "PCB.h"

struct PCB_Priority_Queue {
	PCB_Queue_p queues[PCB_PRIORITY_MAX + 1];
	unsigned int size;
};

typedef struct PCB_Priority_Queue * PCB_Priority_Queue_p;

PCB_Priority_Queue_p PCB_Priority_Queue_construct(enum PCB_ERROR*);
void PCB_Priority_Queue_destruct(PCB_Priority_Queue_p, enum PCB_ERROR *);
void PCB_Priority_Queue_enqueue(PCB_Priority_Queue_p, PCB_p, enum PCB_ERROR*);
PCB_p PCB_Priority_Queue_dequeue(PCB_Priority_Queue_p, enum PCB_ERROR*); 
void PCB_Priority_Queue_print(PCB_Priority_Queue_p, enum PCB_ERROR*);
unsigned int get_PQ_size(PCB_Priority_Queue_p, enum PCB_ERROR*);