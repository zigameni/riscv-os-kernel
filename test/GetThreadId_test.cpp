//
// Created by os on 8/29/25.
//
#include "../h/syscall_c.hpp"
#include "GetThreadId_test.hpp"
#include "printing.hpp"

static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;

static void workerBodyA(void* arg) {
    for (uint64 i = 0; i < 10; i++) {
        int threadId = getThreadId();
        printString("A: i=");
        printInt(i);
        printString(", ID=");
        printInt(threadId);
        printString("\n");

        // Sleep for 0.5 seconds (assuming time_sleep uses ticks, adjust as needed)
        time_sleep(50); // Adjust this value based on your timer implementation
    }
    printString("A finished!\n");
    finishedA = true;
}

static void workerBodyB(void* arg) {
    for (uint64 i = 0; i < 10; i++) {
        int threadId = getThreadId();
        printString("B: i=");
        printInt(i);
        printString(", ID=");
        printInt(threadId);
        printString("\n");

        // Sleep for 0.5 seconds
        time_sleep(50);
    }
    printString("B finished!\n");
    finishedB = true;
}

static void workerBodyC(void* arg) {
    for (uint64 i = 0; i < 10; i++) {
        int threadId = getThreadId();
        printString("C: i=");
        printInt(i);
        printString(", ID=");
        printInt(threadId);
        printString("\n");

        // Sleep for 0.5 seconds
        time_sleep(50);
    }
    printString("C finished!\n");
    finishedC = true;
}

void GetThreadId_test() {
    thread_t threads[3];

    printString("Creating thread A...\n");
    thread_create(&threads[0], workerBodyA, nullptr);
    printString("Thread A created\n");

    printString("Creating thread B...\n");
    thread_create(&threads[1], workerBodyB, nullptr);
    printString("Thread B created\n");

    printString("Creating thread C...\n");
    thread_create(&threads[2], workerBodyC, nullptr);
    printString("Thread C created\n");

    printString("All threads created. Waiting for completion...\n");

    while (!(finishedA && finishedB && finishedC)) {
        thread_dispatch();
    }

    printString("All threads completed!\n");
}