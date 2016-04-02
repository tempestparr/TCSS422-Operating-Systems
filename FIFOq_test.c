/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   FIFOq_test.c
 * Author: Mark
 *
 * Created on April 2, 2016, 11:22 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "FIFOq.h"

/*
 * Simple C Test Suite
 */

void testIs_null() {
    void* this_;
    int* ptr_error;
    int result = is_null(this_, ptr_error);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testIs_null (FIFOq_test) message=error message sample\n");
    }
}

void testFIFOq_construct() {
    int* p0;
    FIFOq_p result = FIFOq_construct(p0);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFIFOq_construct (FIFOq_test) message=error message sample\n");
    }
}

void testFIFOq_destruct() {
    FIFOq_p p0;
    int* p1;
    FIFOq_destruct(p0, p1);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFIFOq_destruct (FIFOq_test) message=error message sample\n");
    }
}

void testFIFOq_init() {
    FIFOq_p p0;
    int* p1;
    int result = FIFOq_init(p0, p1);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFIFOq_init (FIFOq_test) message=error message sample\n");
    }
}

void testFIFOq_is_empty() {
    FIFOq_p p0;
    int* p1;
    int result = FIFOq_is_empty(p0, p1);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFIFOq_is_empty (FIFOq_test) message=error message sample\n");
    }
}

void testFIFOq_enqueue() {
    FIFOq_p p0;
    Node_p p1;
    int* p2;
    FIFOq_enqueue(p0, p1, p2);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFIFOq_enqueue (FIFOq_test) message=error message sample\n");
    }
}

void testFIFOq_dequeue() {
    FIFOq_p p0;
    int* p1;
    PCB_p result = FIFOq_dequeue(p0, p1);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFIFOq_dequeue (FIFOq_test) message=error message sample\n");
    }
}

void testFIFOq_toString() {
    FIFOq_p p0;
    char* p1;
    int p2;
    int* p3;
    char* result = FIFOq_toString(p0, p1, p2, p3);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFIFOq_toString (FIFOq_test) message=error message sample\n");
    }
}

void testNode_construct() {
    PCB_p data;
    Node_p next;
    int* ptr_error;
    Node_p result = Node_construct(data, next, ptr_error);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testNode_construct (FIFOq_test) message=error message sample\n");
    }
}

void testNode_init() {
    Node_p this_;
    PCB_p p1;
    Node_p p2;
    int* p3;
    int result = Node_init(this_, p1, p2, p3);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testNode_init (FIFOq_test) message=error message sample\n");
    }
}

void testNode_destruct() {
    Node_p this_;
    int result = Node_destruct(this_);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testNode_destruct (FIFOq_test) message=error message sample\n");
    }
}

void testNode_getData() {
    Node_p this_;
    int* ptr_error;
    PCB_p result = Node_getData(this_, ptr_error);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testNode_getData (FIFOq_test) message=error message sample\n");
    }
}

void testNode_setData() {
    Node_p this_;
    PCB_p data;
    int result = Node_setData(this_, data);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testNode_setData (FIFOq_test) message=error message sample\n");
    }
}

void testNode_getNext() {
    Node_p this_;
    int* ptr_error;
    Node_p result = Node_getNext(this_, ptr_error);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testNode_getNext (FIFOq_test) message=error message sample\n");
    }
}

void testNode_setNext() {
    Node_p this_;
    Node_p next;
    int result = Node_setNext(this_, next);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testNode_setNext (FIFOq_test) message=error message sample\n");
    }
}

void testNode_toString() {
    Node_p this_;
    char* str;
    int* ptr_error;
    char* result = Node_toString(this_, str, ptr_error);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testNode_toString (FIFOq_test) message=error message sample\n");
    }
}

int FIFOq_test_main(int argc, char** argv) {
    int error = 0;
    int idx = 0;
    int stz = 256;
    char str[stz];
    FIFOq_p fiq = FIFOq_construct(&error);
    int r = rand()%20 + 10;
    printf("!!! %s   %d\n", FIFOq_toString(fiq, str, stz, &error), r);
   
    for (idx = 0; idx < r; idx++)
    {
        FIFOq_enqueue(fiq, Node_construct(PCB_construct(&error), NULL, &error), &error);
        char pcbstr[128];
        printf("%s : contents: %s\n", FIFOq_toString(fiq, str, stz, &error),
                PCB_toString(FIFOq_last_pcb(fiq, &error), pcbstr, &error));
    }    
    
    while (!FIFOq_is_empty(fiq, &error))
    {
        PCB_p pcb = FIFOq_dequeue(fiq, &error);
        char pcbstr[128];
        printf("%s\ncontents: %s\n", FIFOq_toString(fiq, str, stz, &error),
                PCB_toString(pcb, pcbstr, &error));
        PCB_destruct(pcb);
        
    }

    FIFOq_destruct(fiq, &error);
    
    if (1) return 0; //below tests haven't had test conditions
    printf("%%SUITE_STARTING%% FIFOq_test\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%%  testIs_null (FIFOq_test)\n");
    testIs_null();
    printf("%%TEST_FINISHED%% time=0 testIs_null (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testFIFOq_construct (FIFOq_test)\n");
    testFIFOq_construct();
    printf("%%TEST_FINISHED%% time=0 testFIFOq_construct (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testFIFOq_destruct (FIFOq_test)\n");
    testFIFOq_destruct();
    printf("%%TEST_FINISHED%% time=0 testFIFOq_destruct (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testFIFOq_init (FIFOq_test)\n");
    testFIFOq_init();
    printf("%%TEST_FINISHED%% time=0 testFIFOq_init (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testFIFOq_is_empty (FIFOq_test)\n");
    testFIFOq_is_empty();
    printf("%%TEST_FINISHED%% time=0 testFIFOq_is_empty (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testFIFOq_enqueue (FIFOq_test)\n");
    testFIFOq_enqueue();
    printf("%%TEST_FINISHED%% time=0 testFIFOq_enqueue (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testFIFOq_dequeue (FIFOq_test)\n");
    testFIFOq_dequeue();
    printf("%%TEST_FINISHED%% time=0 testFIFOq_dequeue (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testFIFOq_toString (FIFOq_test)\n");
    testFIFOq_toString();
    printf("%%TEST_FINISHED%% time=0 testFIFOq_toString (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testNode_construct (FIFOq_test)\n");
    testNode_construct();
    printf("%%TEST_FINISHED%% time=0 testNode_construct (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testNode_init (FIFOq_test)\n");
    testNode_init();
    printf("%%TEST_FINISHED%% time=0 testNode_init (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testNode_destruct (FIFOq_test)\n");
    testNode_destruct();
    printf("%%TEST_FINISHED%% time=0 testNode_destruct (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testNode_getData (FIFOq_test)\n");
    testNode_getData();
    printf("%%TEST_FINISHED%% time=0 testNode_getData (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testNode_setData (FIFOq_test)\n");
    testNode_setData();
    printf("%%TEST_FINISHED%% time=0 testNode_setData (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testNode_getNext (FIFOq_test)\n");
    testNode_getNext();
    printf("%%TEST_FINISHED%% time=0 testNode_getNext (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testNode_setNext (FIFOq_test)\n");
    testNode_setNext();
    printf("%%TEST_FINISHED%% time=0 testNode_setNext (FIFOq_test)\n");

    printf("%%TEST_STARTED%%  testNode_toString (FIFOq_test)\n");
    testNode_toString();
    printf("%%TEST_FINISHED%% time=0 testNode_toString (FIFOq_test)\n");

    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}
