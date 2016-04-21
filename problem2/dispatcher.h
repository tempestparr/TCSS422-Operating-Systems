/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <stdio.h>

#include "FIFOq.h"
#include "PCB.h"
#include "CPU.h"

void dispatcher(FIFOq_p readyQ, PCB current, int* error);

#endif /* DISPATCHER_H_ */
