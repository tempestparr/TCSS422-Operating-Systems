/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 */

#pragma once

#include "PCB.h"
#include "mutex.h"
#include "PCB_Priority_Queue.h"

typedef struct condition_var_type {
	PCB_Queue_p waiting;
    unsigned int flag;
    unsigned int id;
} Condition_Variable;

typedef struct condition_var_type * Condition_p;

Condition_p Condition_construct(unsigned int id, enum PCB_ERROR *);
void Condition_destruct(Condition_p, enum PCB_ERROR *);
int Condition_wait(Condition_p, Mutex_p, PCB_Priority_Queue_p, enum PCB_ERROR *);
void Condition_signal(Condition_p, PCB_Priority_Queue_p, enum PCB_ERROR *);