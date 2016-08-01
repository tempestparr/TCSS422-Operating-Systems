# TCSS422

## Problem 1 Objectives:

- To design, implement, and test the data structures needed to manage scheduling priority processes in the kernel of the OS.
- To refresh your understanding of Abstract Data Types (ADTs) and their relation to object oriented design and programming.
- To learn how to do Object Oriented Programming in C.

> Design and implement three abstract data types (ADT) used in OS scheduling. The ADTs will be developed incrementally, starting with the process control block structure and its operations. Design and implement a FIFO queue to hold pointers to various PCBs that are to be instantiated in dynamic memory. The final ADT will be for a priority queue that will use multiple instantiations of the FIFO queues.


## Problem 2 Objective:

- To design, implement, and test a scheduling simulation (with context switching) using the Round Robin algorithm and the data structures/methods previously developed.

> Design a scheduler that only implements the round robin algorithm.

## Problem 3 Objectives: 

- To learn how to handle I/O service request traps and I/O service completion interrupts. 
- To see the effects of discontinuities (asynchronous events) on scheduling.

> In this simulation each iteration represents a single instruction executing. With each iteration you will be incrementing  the PC of the running process by one. The timer interrupt will occur on regular intervals. The PCB will need to be augmented in order to simulate the I/O service traps.

## Final Project Objective:

- To complete the OS simulator by adding a few more things to the code you have already built and doing some experiments to see differences in behaviors under different conditions.

> Requirements:

> - Create four priority levels and populate the levels roughly with the following percentages: 0 - 5%, 1 - 80%, 2 - 10%, 3 - 5%.

> - Implement a starvation prevention mechanism that detects when lower level priority processes are not getting CPU time and temporarily boost their priority to the next higher level for a quantum.

> - Simulate synchronization services in order to implement coordination between various processes. Specifically, implement mutexes and condition variables based on the pthread versions.

> - Build several more arrays into the kinds of processes that will be using synchronization tools. These arrays will contain the step numbers when these processes execute traps to lock, or trylock, or unlock a mutex. They will also contain steps when processes wait or signal condition variables.

> - Set up some Producer-Consumer pairs. In each pair one process will be a producer and the other will be a consumer. To allow the pair to communicate, a global integer is read from and written to by the pair. The producer will increment a variable each time through its cycle.

> - Mutual Resource User processes should also be created in pairs but there are two versions to be used in experiments on deadlock. The objective is to get two or more processes that attempt to acquire the same resources. In one version we simulate the correct situation of no circular waiting to prevent deadlocks. In the other run we set up the locking steps so that we permit all four conditions for deadlock. There is no guarantee that deadlock will occur but over several runs of the simulation you should see some processes in deadlock. 

> - Special Deadlock Monitor is a function not part of a regular scheduler but for simulation to determine if processes in the waiting queues (R1 and R2) are stuck due to circular waiting.
