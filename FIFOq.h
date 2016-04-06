/*
 * Problem 1 - OS Abstract Data Types
 * TCSS 422 A Spring 2016
 * Christopher Ottersen, Tempest Parr, Mark Peters
 */

#ifndef FIFOQ_H
#define FIFOQ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"

#define this this_
#define FIFO_NULL_ERROR 11
#define FIFO_CONSTRUCT_ERROR 13
#define FIFO_STRING_ERROR 17
#define NODE_NULL_ERROR 23
#define NODE_CONSTRUCT_ERROR 29
#define NODE_STRING_ERROR 31
#define NODE_DATA_ERROR 37

typedef struct FIFOq * FIFOq_p;
typedef struct Node_type Node;
typedef Node * Node_p;

struct FIFOq
{
    int size;
    Node_p head;
    Node_p tail;
};

struct Node_type
{
  int pos;
  PCB_p data;
  Node_p next_node;
};

FIFOq_p FIFOq_construct(int*);
void    FIFOq_destruct (FIFOq_p, int*);              // deallocates pcb from the heap
int     FIFOq_init     (FIFOq_p, int*);              // sets default values for member data
int     FIFOq_is_empty (FIFOq_p, int*);
void    FIFOq_enqueue  (FIFOq_p, Node_p, int*);
PCB_p   FIFOq_dequeue  (FIFOq_p, int*);
//PCB_p   FIFOq_last_pcb (FIFOq_p, int*);
char *  FIFOq_toString (FIFOq_p, char*, int*, int*); // returns a string representing the contents of the pcb

Node_p Node_construct(PCB_p data, Node_p next, int * ptr_error);
int    Node_init     (Node_p this, PCB_p, Node_p, int*);
int    Node_destruct (Node_p this);
PCB_p  Node_getData  (Node_p this, int * ptr_error);
int    Node_setData  (Node_p this, PCB_p data);
Node_p Node_getNext  (Node_p this, int * ptr_error);
int    Node_setNext  (Node_p this, Node_p next);
char * Node_toString (Node_p this, char *str, int *ptr_error);

int is_null(void *this, int *ptr_error);
int FIFOq_test_main(int argc, char** argv);

#endif /* FIFOQ_H */


