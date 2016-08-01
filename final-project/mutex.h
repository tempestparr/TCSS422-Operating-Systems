/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 */

#pragma once

#include "PCB_Queue.h"
#include "PCB_Priority_Queue.h"
#include "PCB.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct mutex_type {
	PCB_p owner;
	PCB_Queue_p waiting;
	unsigned int is_locked;
	unsigned int id;
} Mutex;

typedef Mutex * Mutex_p;

Mutex_p Mutex_construct(int, enum PCB_ERROR *);
void Mutex_destruct(Mutex_p, enum PCB_ERROR *);
int Mutex_lock(Mutex_p, PCB_p, enum PCB_ERROR *);
int Mutex_trylock(Mutex_p, PCB_p, enum  PCB_ERROR *);
int Mutex_unlock(Mutex_p, PCB_p, PCB_Priority_Queue_p, enum PCB_ERROR *);
