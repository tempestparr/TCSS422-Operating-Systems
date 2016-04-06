/*
 * Problem 1 - OS Abstract Data Types
 * TCSS 422 A Spring 2016
 * Christopher Ottersen, Tempest Parr, Mark Peters
 */

#ifndef PRIORITYQ_H
#define PRIORITYQ_H

#include "FIFOq.h"

#define PRIORITY_Q_STRING_BUFFER_SIZE 8192
#define PRIORITIES_TOTAL 16

typedef FIFOq_p * PriorityQ_p;

//
// Member functions
//
PriorityQ_p PriorityQ_construct();
void        PriorityQ_destruct (PriorityQ_p readyQ);
bool        PriorityQ_is_empty (PriorityQ_p readyQ);
void        PriorityQ_enqueue  (PriorityQ_p readyQ, PCB_p pcbToEnqueue);
PCB_p       PriorityQ_dequeue  (PriorityQ_p readyQ);
char*       PriorityQ_toString (PriorityQ_p readyQ);
void        PriorityQ_test_main();

#endif