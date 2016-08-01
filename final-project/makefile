CFLAGS=-Wall -g

all: Thread_Sim

Thread_Sim: Thread_Sim.o PCB.o PCB_Queue.o mutex.o cond_var.o PCB_Priority_Queue.o
	gcc -pthread Thread_Sim.o PCB.o PCB_Queue.o PCB_Priority_Queue.o mutex.o cond_var.o -o Thread_Sim

Thread_Sim.o: Thread_Sim.c
	gcc $(CFLAGS) -c Thread_Sim.c
mutex.o: mutex.c
	gcc $(CFLAGS) -c mutex.c
cond_var.o: cond_var.c
	gcc $(CFLAGS) -c cond_var.c
PCB_Queue.o: PCB_Queue.c
	gcc $(CFLAGS) -c PCB_Queue.c
PCB_Priority_Queue.o: PCB_Priority_Queue.c
	gcc $(CFLAGS) -c PCB_Priority_Queue.c 
PCB.o: PCB.c
	gcc $(CFLAGS) -c PCB.c