/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FIFOq.h
 * Author: Mark
 *
 * Created on April 1, 2016, 12:11 PM
 */

#ifndef FIFOQ_H
#define FIFOQ_H

#import "PCB.h"


struct FIFOq {
    int size;
    FIFOq_p head;
};

struct Node {
    PCB_p pcb;
    Node_p next;
};

typedef FIFOq * FIFOq_p;
typedef Node * Node_p;

FIFOq_p FIFOq_construct(void);
int FIFOq_destruct (FIFOq_p);  // deallocates pcb from the heap
int FIFOq_init (FIFOq_p);       // sets default values for member data
int FIFOq_is_empty(FIFOq_p);
int FIFOq_enqueue(FIFOq_p, Node_p);
int FIFOq_dequeue(FIFOq_p);
char * FIFOq_toString (FIFOq_p);  // returns a string representing the contents of the pcb

#endif /* FIFOQ_H */


