/*
 * Problem 1 - OS Abstract Data Types
 * TCSS 422 A Spring 2016
 * Christopher Ottersen, Tempest Parr, Mark Peters
 */

#include "FIFOq.h"

/* Constructs you a boring empty FIFO queue and also intializes it.
 * 
 * error is the error pointer
 * 
 * returns you a boring empty FIFO queue
 */
FIFOq_p FIFOq_construct(int *error) {
    FIFOq_p this = (FIFOq_p) malloc(sizeof(struct FIFOq));
    int init_error = ((!this) * FIFO_CONSTRUCT_ERROR) || FIFOq_init(this, error);
    if (init_error && !is_null(this, error)) {
        FIFOq_destruct(this, error);
    }
    return this;
}

/* Utterly destroys the FIFO queue and everything in it.
 * 
 * this is the FIFO queue no more
 * error is the error pointer
 */
void FIFOq_destruct(FIFOq_p this, int *error) {
    if (!is_null(this, error)) {
        Node_p node = this->head;
        while (this->head != NULL) {
            node = this->head->next_node;
            PCB_destruct(this->head->data);
            Node_destruct(this->head);
            this->head = node;
        }
        free(this);
        this = NULL;
    }
}

/* Initializes the values of the FIFO queue.
 * 
 * this is the FIFO queue that is hopelessly lost without values
 * error is the error pointer and everything that is wrong with society
 * 
 * returns an error int
 */
int FIFOq_init(FIFOq_p this, int *error) {
    if (!is_null(this, error)) {
        this->size = 0;
        this->head = NULL;
    }
    return error == NULL ? 0 : *error;
}

/* Checks if the queue is empty and tells you politely.
 * 
 * this is the queue that may or may not be empty
 * error is the error pointer
 * 
 * returns the error as a plain boring int
 */
int FIFOq_is_empty(FIFOq_p this, int *error) {
    if (!is_null(this, error)) {
        return this->head == NULL;
    }
    return error == NULL ? 0 : *error;
}

/* Enqueues the given node to the end (tail) of the queue, unless the queue is
 * empty, in which case it enqueues to the head.
 * 
 * this is the FIFO queue whose beautiful little village is growing by one
 * next is the node to queue into the FIFO queue
 * error is the pointer error
 * 
 * returns nothing because too bad
 */
void FIFOq_enqueue(FIFOq_p this, Node_p next, int *error) {
    if (!is_null(this, error) && !is_null(next, error)) {

        // an enqueued PCB is in the ready state
        PCB_setState(next->data, 1);

        if (this->head == NULL) {
            if (!next->pos) {
                next->pos = 1;
            }
            this->head = next;
            this->tail = this->head;
        } else {
            if (!next->pos) {
                next->pos = this->tail->pos + 1;
            }
            this->tail->next_node = next;
            this->tail = next;
        }
        this->size++;
    }
}

/* Dequeues the node located at the head of the queue, destroying the node and
 * returning the pcb pointer. The head of the fifo queue is re-chosen to be
 * the next_node of the former head node.
 * 
 * this is the FIFO queue we are dequeueing from
 * error is the error pointer
 * 
 * returns the pcb pointer that was the head
 */
PCB_p FIFOq_dequeue(FIFOq_p this, int *error) {
    if (!is_null(this, error) && !is_null(this->head, error)) {
        Node_p node = this->head;
        this->head = this->head->next_node;
        if (this->head == NULL) {
            this->tail = NULL;
        }
        this->size--;
        PCB_p pcb = node->data;

        // a dequeued PCB is in the ready state
        PCB_setState(pcb, 2);

        Node_destruct(node);
        return pcb;
    }
    return NULL;
}

/* FIFO toString command, which gives an output of each element in the queue
 * with their node number plus the toString of the last pcb.
 * 
 * this is the FIFO queue whose toString we're constructing
 * str is the string to place the toString in
 * stz is the size of that string bugger
 * error is the error pointer
 * 
 * return a version of the toString as a null-terminated char pointer
 */
char * FIFOq_toString(FIFOq_p this, char *str, int *stz, int *error) {
    int usedChars = 0;
    if (!is_null(this, error) && !is_null(str, error)) {
        usedChars += snprintf(str, *stz - usedChars, "Count=%d:", this->size);
        if (this->head != NULL) {
            Node_p node = this->head;
            while (node != NULL) {
                PCB_p pcb = node->data;
                unsigned long pid = PCB_getPid(pcb, NULL);
                usedChars += snprintf(str + strlen(str), *stz - usedChars, "%cP%lu-", node == this->head? ' ' : '>', pid);
                node = node->next_node;
            }
            usedChars += snprintf(str + strlen(str), *stz - usedChars, "*");
        }
        *stz = *stz - usedChars;
        return str;
    } else {
      return 0;
    }
}

/* Constructs a node with the given parameters. For internal use only within
 * the FIFO queue so thus acting as an inner class.
 * 
 * data is the pcb to stick in the node
 * next_node is the linked node that follows the current one
 * error is the error pointer
 * 
 * returns the node, duh
 */
Node_p Node_construct (PCB_p data, Node_p next_node, int *error) {
    Node_p this = (Node_p) malloc(sizeof(struct Node_type));
    int init_error = ((!this) * NODE_CONSTRUCT_ERROR) ||
                     Node_init(this, data, next_node, error);
    init_error = init_error + (this == NULL) * NODE_NULL_ERROR;
    if (init_error) {
        Node_destruct(this);
    }
    return this;
}

/* Node data initializer that sets the node with the given values.
 * 
 * this is the node we're initializing
 * data is the pcb the node points to
 * next_node is the subsequent node the node points to
 * error is the error integer pointer
 * 
 * returns an error integer
 */
int Node_init (Node_p this, PCB_p data, Node_p next_node, int *error) {
    int init_error = (this == NULL) * NODE_NULL_ERROR;
    if (error != NULL)
        *error += init_error;
    if (!init_error) {
        this->pos = 0;
        this->data = data;
        this->next_node = next_node;
    }
    return error == NULL ? init_error : *error;
}

/* Deallocates and nullifies the given node.
 * 
 * this is the node we hate so much it's got to go
 * 
 * return an integer error pointer
 */
int Node_destruct (Node_p this) {
  int error = (this == NULL) * NODE_NULL_ERROR;
  if (!error) {
    free(this);
    this = NULL;
  }
  return error;
}

/* Gets the data (pcb) of the node and returns it.
 * 
 * this is the node whose data we're reading
 * ptr_error is the error pointer
 * 
 * return the data (pcb) that the given node was pointing to
 */
PCB_p Node_getData (Node_p this, int * ptr_error) {
  int error = this == NULL;
  if (ptr_error != NULL) {
    *ptr_error = error;
  }
  return error ? NULL : this->data;
}

/* Sets the node's data (pcb), overwriting any prior data.
 * 
 * this is the node whose data we're writing to
 * data is the pcb pointer we're giving to the node
 * 
 * returns the error integer
 */
int Node_setData (Node_p this, PCB_p data) {
  int error = (this == NULL) * NODE_NULL_ERROR;
  if (!error) {
    this->data = data;
  }
  return error;
}

/* Returns the next node of the given one.
 * 
 * this is the node we want the next node of
 * ptr_error is the error pointer
 * 
 * returns a pointer to the next node
 */
Node_p Node_getNext (Node_p this, int * ptr_error) {
  int error = (this == NULL) * NODE_NULL_ERROR;
  if (ptr_error != NULL) {
    *ptr_error += error;
  }
  return error ? NULL : this->next_node;
}

/* Sets the next node with the given one.
 * 
 * this is the node whose next pointer is changing
 * next_node is the node the this node will point t
 * 
 * return an integer representing the error state
 */
int Node_setNext (Node_p this, Node_p next_node) {
  int error = (this == NULL) * NODE_NULL_ERROR;
  if (!error) {
    this->next_node = next_node;
  }
  return error;
}

/* Returns a string describing the node. Not used anymore as PCB's toString
 * is all that is needed for display.
 * 
 * this is the node reference
 * str is the input str for storing print data
 * ptr_error is the error pointer
 * 
 * returns the tostring
 */
char * Node_toString (Node_p this, char *str, int *ptr_error) {
  int error = ((this == NULL) * NODE_NULL_ERROR |
    (NODE_STRING_ERROR * (str == NULL)) |
    (NODE_DATA_ERROR * (this->data == NULL)));
  
  if (ptr_error != NULL) {
    *ptr_error += error;
  }

  if (!error) {
    snprintf(str, 10, "P%lx", PCB_getPid(this->data, NULL));
  }
  
  return str;
}

/* Checks if the given reference is null and updates error pointer if so.
 * Note, Node was written before this idea so node has its function-dependent
 * error checking.
 * 
 * this is the unknown type we're checking for nullness
 * ptr_error is the error pointer
 * 
 * returns the error as an int as well for boolean tests
 */
int is_null(void *this, int *ptr_error) {
    int error = (this == NULL) * FIFO_NULL_ERROR;
    if (ptr_error != NULL) {
        *ptr_error = error + *ptr_error;
    }
    return ptr_error == NULL ? error : error + *ptr_error;
}
