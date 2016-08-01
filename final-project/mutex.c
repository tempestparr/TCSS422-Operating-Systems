/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 */

#include "mutex.h"

/**
 * Creates new Mutex and returns pointer to it.
 */
Mutex_p Mutex_construct(int id, enum PCB_ERROR *error) {
	Mutex_p m = malloc(sizeof(Mutex));
	if (m == NULL) {
		*error = PCB_MEM_ALLOC_FAIL;
		return NULL;
	}
    m->owner = NULL;
	m->is_locked = 0;
	m->id = id;
	m->waiting = PCB_Queue_construct(error);
	return m;
}

/**
 * Frees the memory taken up by the mutex.
 */
void Mutex_destruct(Mutex_p m, enum PCB_ERROR *error) {
	PCB_Queue_destruct(m->waiting, error);
	free(m);
}

/*
 * Attempts to lock the mutex, will put PCB in waiting queue if already locked.
 */
int Mutex_lock(Mutex_p m, PCB_p pcb, enum PCB_ERROR *error) {
	if (m->is_locked && pcb != m->owner) {
        if (PCB_Queue_is_empty(m->waiting, error) ||
            ((!PCB_Queue_is_empty(m->waiting, error)) && pcb != m->waiting->first_node_ptr->value)) {
            PCB_Queue_enqueue(m->waiting, pcb, error);
            printf("MUTEX_LOCK_%d: placing PID 0x%lX into waitQ:\n", m->id, pcb->pid);
        } else {
            printf("MUTEX_LOCK_%d:  PID 0x%lX already in waitQ:\n", m->id, pcb->pid);
        }
        PCB_Queue_print(m->waiting, error);
        return 1;
	} else {
		m->owner = pcb;
		m->is_locked = 1;
        return 0;
	}
}

/**
 * Checks if the mutex is locked and will return 0 if it already is locked, otherwise
 * it will lock the mutex with the pcb.
 */
int Mutex_trylock(Mutex_p m, PCB_p pcb, enum PCB_ERROR *error) {
	if (m->is_locked) {
		return 1;
	} else {
        m->owner = pcb;
		m->is_locked = 1;
        return 0;
    }
}

/**
 * Attempts to unlock the mutex.
 */
int Mutex_unlock(Mutex_p m, PCB_p pcb, PCB_Priority_Queue_p readyQueue, enum PCB_ERROR *error) {
	if (m->is_locked && m->owner == pcb) {
		if (PCB_Queue_is_empty(m->waiting, error)) {
			m->is_locked = 0;
			m->owner = NULL;
		} else {
			m->owner = PCB_Queue_dequeue(m->waiting, error);	// If PCB waiting, it will get the lock.
			PCB_Priority_Queue_enqueue(readyQueue, m->owner, error);
            printf("MUTEX_UNLOCK_%d: new owner PID 0x%lX, waitQ:\n", m->id, m->owner->pid);
            PCB_Queue_print(m->waiting, error);
		}
        return 0;
	}
    return 1;
}
