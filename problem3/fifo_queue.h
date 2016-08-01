/*
 *  TCSS 422 Spring 2016
 *  Team 4 Luis Solis-Bruno, Mat Sharff, Tempest Parr, Sara Vandandaigue
 *  Problem 3 Discontinuities
 */

#ifndef FIFO_QUEUE_H
#define FIFO_QUEUE_H

#include "pcb.h"

typedef struct node Node;
typedef Node * Node_p;

struct node {
    PCB_p pcb;
    Node_p next;
};

typedef struct FIFO_q {
    int size;
    Node_p front;
    Node_p rear;
} FIFOq;
typedef FIFOq * FIFOq_p;

FIFOq_p FIFOq_construct(void);
int FIFOq_destruct(FIFOq_p fqueue);
int FIFOq_init(FIFOq_p queue);
int FIFOq_is_empty(FIFOq_p queue);
int FIFOq_size(FIFOq_p queue);
int FIFOq_enqueue(FIFOq_p queue, PCB_p pcb);
PCB_p FIFOq_dequeue(FIFOq_p queue);
PCB_p FIFOq_peek(FIFOq_p queue);
char* FIFOq_toString(FIFOq_p queue, char * string, int size);

#endif
