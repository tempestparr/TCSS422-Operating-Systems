/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 *
 */

#pragma once

#include "PCB.h"

typedef struct PCB_Queue * PCB_Queue_p; // queue pointer for malloc

struct node {
	PCB_p value; 
	struct node * next_node;
};

struct PCB_Queue {
	struct node * first_node_ptr;
	struct node * last_node_ptr;
	int size;
};

PCB_Queue_p PCB_Queue_construct(enum PCB_ERROR*);
void PCB_Queue_destruct(PCB_Queue_p, enum PCB_ERROR*);
int PCB_Queue_is_empty(PCB_Queue_p, enum PCB_ERROR*);
void PCB_Queue_enqueue(PCB_Queue_p, PCB_p, enum PCB_ERROR*); 
PCB_p PCB_Queue_dequeue(PCB_Queue_p, enum PCB_ERROR*);
void PCB_Queue_print(PCB_Queue_p, enum PCB_ERROR*);
