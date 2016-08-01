/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 *
 */

#include "PCB_Queue.h"
#include "PCB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

PCB_Queue_p PCB_Queue_construct(enum PCB_ERROR *error) {
	PCB_Queue_p list = malloc(sizeof(struct PCB_Queue));
	if (list == NULL) {
		*error = PCB_MEM_ALLOC_FAIL;
		return NULL;
	}
	list->first_node_ptr = NULL;
	list->last_node_ptr = NULL;
	list->size = 0;
	return list;
}

void PCB_Queue_destruct(PCB_Queue_p theList, enum PCB_ERROR *error) {
	if (theList->size != 0) {
		struct node* current_ptr = theList->first_node_ptr->next_node;
		struct node* temp_ptr = theList->first_node_ptr;
		int i;
		for (i = 0; i < theList->size; i++) {
			if (current_ptr == NULL) { // the element is only one left
				free(temp_ptr->value);
				temp_ptr->value = NULL;
			}
			else{
				free(temp_ptr->value);
				temp_ptr->value = NULL;
				temp_ptr = current_ptr; //same as dequeue
				current_ptr = temp_ptr->next_node;
			}
		}
	}
	free(theList);
	theList = NULL;
}

int PCB_Queue_is_empty(PCB_Queue_p theList, enum PCB_ERROR *error) {
	if (theList == NULL) {
		*error = PCB_NULL_POINTER;
		return -1;
	}
	return theList->size == 0;
}

void PCB_Queue_enqueue(PCB_Queue_p theList, PCB_p theValue, enum PCB_ERROR *error) {
	if (theList == NULL || theValue == NULL ) {
		*error = PCB_NULL_POINTER;
		return;
	}

	struct node* temp_Node = malloc(sizeof(struct node));
	temp_Node->value = theValue;
	temp_Node->next_node = NULL;

	if (PCB_Queue_is_empty(theList, error)) {
		theList->first_node_ptr = temp_Node;	
	} else {
		theList->last_node_ptr->next_node = temp_Node;
	}
	theList->last_node_ptr = temp_Node;
	theList->size++;  
}

PCB_p PCB_Queue_dequeue(PCB_Queue_p theList, enum PCB_ERROR *error) {
	if (theList == NULL) {
		*error = PCB_NULL_POINTER;
		return NULL;
	}
	if (PCB_Queue_is_empty(theList, error)) {
		*error = PCB_INVALID_ARG;
		return NULL;
	}
	struct node * tempNode = theList->first_node_ptr; 
	if (tempNode->next_node != NULL) {
		theList->first_node_ptr = tempNode->next_node;
	} else {
		theList->first_node_ptr = NULL;
		theList->last_node_ptr = NULL;
	}
	theList->size--;
	PCB_p p = tempNode->value;
	free(tempNode);
	return p;
}

// print for check 
void PCB_Queue_print(PCB_Queue_p theList, enum PCB_ERROR *error) {
	printf("Size = %d. ", theList->size);
	struct node* n = theList->first_node_ptr;
	while (n != NULL) {
		printf("0x%lX->", n->value->pid);
		n = n->next_node;
	}
	printf("\n");
}