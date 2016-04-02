/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "FIFOq.h"


FIFOq_p FIFOq_construct(int * ptr_error)
{
    FIFOq_p this = (FIFOq_p) malloc(sizeof(struct FIFOq));
    int error = ((!this) * 4) || FIFOq_init(this);
    if(ptr_error != NULL)
    {
        *ptr_error = error;
    }
    if (error)
    {
        this->size = NULL;
        this->head = NULL;
    }
    return this;

}

int FIFOq_destruct (FIFOq_p this)
{
  int error = this == NULL;
  if (!error)
  {
      Node_p node = this->head;
      while (this->head != NULL)
      {
          node = this->head->next_node;
        Node_destruct(this->head);
        this->head = node;
      }
    free(this);
    this = NULL;
  }
  return error;
}


int FIFOq_init (FIFOq_p this)
{
  int error = this == NULL;
  if (!error)
  {
    this->size = 0;
    this->head = NULL;
  }
  return error;

}// sets default values for member data


int FIFOq_is_empty(FIFOq_p this)
{
  int error = this == NULL;
  if (!error)
  {
    return this->head == NULL;
  }
  return error * -1;  
}

int FIFOq_enqueue(FIFOq_p this, Node_p next)
{
    int error = this == NULL;
  if (!error)
  {
      if (this->head == NULL)
      {
          if (!next->pos) next->pos = 1;
          this->head = next;
      }
      else
      {
          int pos = 2;
          Node_p node = this->head;
          while (node->next_node != NULL)
        {
              node = node->next_node;
              pos++;
        }
          if (!next->pos) next->pos = pos;
        node->next_node = next;
      }
      this->size++;
  }
  return error;
}

PCB_p FIFOq_dequeue(FIFOq_p this, int * ptr_error)
{
  int error = this == NULL || this->head == NULL;
  if (ptr_error != NULL)
  {
    *ptr_error = error;
  }
  if (!error)
  {
      Node_p node = this->head;
      this->head = this->head->next_node;
      this->size--;
      PCB_p pcb = node->data;
      Node_destruct(node);
      return pcb;
  }
  return NULL;
}

char * FIFOq_toString (FIFOq_p this, char *str, int * ptr_error)
{
     int error = this == NULL;
     if (!error)
     {
         sprintf(str + strlen(str), "Q:Count=%d: ", this->size);
         if (this->head != NULL)
         {
             sprintf(str + strlen(str), " P%d-", this->head->pos);             
             Node_p node = this->head->next_node;
             while (node != NULL)
             {
               sprintf(str + strlen(str), ">P%d-", node->pos);
               node = node->next_node;
            }
            sprintf(str + strlen(str), "*");
         }
         return str;
     }
     else return 0;
}

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
      this->pos = 0;
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
      this->pos = NULL;
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
    free(this);
    this = NULL;
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
 // snprintf(str, 10, "P%x", this->data->pid);
  //netbeans: error: dereferencing pointer to incomplete type 'PCB {aka struct pcb}'
  return str;
}
