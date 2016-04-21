/*
 * Problem 2 - Scheduling Basics
 * TCSS 422 A Spring 2016
 * Tempest Parr, Mark Peters, Andy Tran, Christine Vu
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <stdio.h>

#include "FIFOq.h"
#include "PCB.h"
#include "CPU.h"
#include "dispatcher.h"

void isrTimer(FIFOq_p createQ, FIFOq_p readyQ, PCB current, int* error);

#endif /* INTERRUPT_H_ */
