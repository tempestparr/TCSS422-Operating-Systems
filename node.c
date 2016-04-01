/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Node.c
 * Author: chris
 *
 * Created on April 1, 2016, 2:26 PM
 */

#include <stdlib.h>
#include <string.h>
#include "node.h"
struct Node_type
{
  PCB_p data;
  Node_p next_node;
};

Node_p Node_construct (PCB_p data, Node_p next_node, int * ptr_error)
{
  Node_p this = (Node_p) malloc(sizeof(struct Node_type));
  int error = ((!this) * 4) || Node_init(this);
  if(ptr_error != NULL)
  {
    *ptr_error = error;
  }
  if (!error)
  {
    this->data = data;
    this->next_node = next_node;
  }
  return this;
}

int Node_init (Node_p this)
{
  int error = this == NULL;
  if (!error)
  {
    this->data = NULL;
    this->next_node = NULL;
  }
  return error;
}

int Node_destruct (Node_p this)
{
  int error = this == NULL;
  if (!error)
  {
    free(error);
  }
  return error;
}

PCB_p Node_getData (Node_p this, int * ptr_error)
{
  int error = this == NULL;
  if (ptr_error != NULL)
  {
    *ptr_error = error;
  }
  return error ? NULL : this->data;
}

int Node_setData (Node_p this, PCB_p data)
{
  int error = this == NULL;
  if (!error)
  {
    this->data = data;
  }
  return error;
}

Node_p Node_getNext (Node_p this, int * ptr_error)
{
  int error = this == NULL;
  if (ptr_error != NULL)
  {
    *ptr_error = error;
  }
  return error ? NULL : this->next_node;
}

int Node_setNext (Node_p this, Node_p next_node)
{
  int error = this == NULL;
  if (!error)
  {
    this->next_node = next_node;
  }
  return error;
}

char * Node_toString (Node_p this, char *str, int *ptr_error)
{
  int error = this == NULL | 2 * (str == NULL) | 4 * (this->data == NULL);
  if (ptr_error != NULL)
  {
    *ptr_error = error;
  }
  snprintf(str, 10, "P%x", this->data->pid);
  return str;
}


