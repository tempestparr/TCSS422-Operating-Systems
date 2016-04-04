problem1: main.o FIFOq.o FIFOq_test.o pcb.o PriorityQ.o PriorityQ_test.o
	gcc -o problem1 main.o FIFOq.o FIFOq_test.o pcb.o PriorityQ.o PriorityQ_test.o

main.o:
	gcc -c main.c

FIFOq.o:
	gcc -c FIFOq.c

FIFOq_test.o:
	gcc -c FIFOq_test.c

PriorityQ.o:
	gcc -c PriorityQ.c

pcb.o:
	gcc -c pcb.c

PriorityQ_test.o:
	gcc -c 	PriorityQ_test.c