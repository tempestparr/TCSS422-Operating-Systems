/*
* TCSS 422 Final Project
* Team 4
* Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
*/

#include "cond_var.h"

/*
 * Creates a new condition variable with the given id.
 * Must manually set up starting flag value.
 */
Condition_p Condition_construct(unsigned int id, enum PCB_ERROR *error) {
	Condition_p condition = malloc(sizeof(Condition_Variable));
	condition->waiting = PCB_Queue_construct(error);
    condition->id = id;
	return condition;
}

/**
 * Frees the memory used by this condition variable. 
 */
void Condition_destruct(Condition_p condition, enum PCB_ERROR *error) {
	PCB_Queue_destruct(condition->waiting, error);
	free(condition);
}

/**
 * Will enqueue the process to this condition's waiting queue and switch its state
 * to waiting. 
 */
int Condition_wait(Condition_p condition, Mutex_p mutex, PCB_Priority_Queue_p readyQueue, enum PCB_ERROR *error) {
	// PCB_set_state(mutex->owner, PCB_STATE_WAITING, error);
	// PCB_Queue_enqueue(condition->waiting, mutex->owner, error);
	// Mutex_unlock(mutex, mutex->owner, error);
    PCB_p temp;
    if (condition->flag == 0) {
        if(!PCB_Queue_is_empty(condition->waiting, error)) {
            return 1;
        }
        PCB_set_state(mutex->owner, PCB_STATE_WAITING, error);
        PCB_Queue_enqueue(condition->waiting, mutex->owner, error);
        Mutex_unlock(mutex, mutex->owner, readyQueue, error);
        return 1;
    } else {
        condition->flag = 0;
        if(!PCB_Queue_is_empty(condition->waiting, error)) {
            temp = PCB_Queue_dequeue(condition->waiting, error);
            Mutex_lock(mutex, temp, error);
        }
        return 0;
    }
}

/**
 * "Wakes up" the processes that are waiting for the condition to change.
 */
void Condition_signal(Condition_p condition, PCB_Priority_Queue_p pq, enum PCB_ERROR *error) {
	PCB_p temp;
	while (!PCB_Queue_is_empty(condition->waiting, error)) {
		temp = PCB_Queue_dequeue(condition->waiting, error);
		PCB_set_state(temp, PCB_STATE_READY, error);
        PCB_Priority_Queue_enqueue(pq, temp, error);
	}
    condition->flag = 1;
}