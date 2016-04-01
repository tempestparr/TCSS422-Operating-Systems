/* 
 * File:   node.h
 * Author: chris
 *
 * Created on March 29, 2016, 4:53 PM
 */

#ifndef NODE_H
#define NODE_H
#define this this_
#include "pcb.h"

typedef struct Node_type Node;
typedef Node * Node_p;


Node_p Node_construct (PCB_p data, Node_p next, int * ptr_error);
int Node_init (Node_p this);
int Node_destruct (Node_p this);

PCB_p Node_getData (Node_p this, int * ptr_error);
int Node_setData (Node_p this, PCB_p data);

Node_p Node_getNext (Node_p this, int * ptr_error);
int Node_setNext (Node_p this, Node_p next);

char * Node_toString (Node_p this, char *str, int *ptr_error);


#endif /* NODE_H */

