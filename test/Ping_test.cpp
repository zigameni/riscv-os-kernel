//
// Created by os on 8/30/25.
//

#include "../h/syscall_c.hpp"
#include "Ping_test.hpp"
#include "printing.hpp"

static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;

static thread_t threadA = nullptr;

static void workerBodyA(void* arg) {
    printString("Thread A starting...\n");

    for (uint64 i = 0; i < 11; i++) {
        printString("A: i=");
        printInt(i);
        printString("\n");

        // Allocate some memory to demonstrate memory tracking
        if (i % 3 == 0) {
            void* mem = mem_alloc(128); // Allocate 128 bytes
            if (mem != nullptr) {
                printString("A: Allocated memory block\n");
            }
        }

        // Sleep a bit to allow other threads to run
        time_sleep(10);
    }

    printString("Thread A finished!\n");
    finishedA = true;
}

static void workerBodyB(void* arg) {
    printString("Thread B starting...\n");

    for (uint64 i = 0; i < 11; i++) {
        printString("B: i=");
        printInt(i);
        printString("\n");

        // Allocate some memory
        if (i % 2 == 0) {
            void* mem = mem_alloc(64); // Allocate 64 bytes
            if (mem != nullptr) {
                printString("B: Allocated memory block\n");
            }
        }

        time_sleep(15);
    }

    printString("Thread B finished!\n");
    finishedB = true;
}

static void workerBodyC(void* arg) {
    printString("Thread C starting...\n");

    for (uint64 i = 0; i < 11; i++) {
        printString("C: i=");
        printInt(i);
        printString("\n");

        // When C reaches iteration 5, ping thread A
        if (i == 5) {
            printString("C: Pinging thread A at iteration 5\n");
            int result = ping(threadA);
            if (result == 0) {
                printString("C: Successfully pinged thread A\n");
            } else {
                printString("C: Failed to ping thread A\n");
            }
        }

        // Allocate some memory
        if (i % 4 == 0) {
            void* mem = mem_alloc(96); // Allocate 96 bytes
            if (mem != nullptr) {
                printString("C: Allocated memory block\n");
            }
        }

        time_sleep(20);
    }

    printString("Thread C finished!\n");
    finishedC = true;
}

void Ping_test() {
    printString("=== Ping System Call Test ===\n");

    thread_t threads[3];

    printString("Creating thread A...\n");
    thread_create(&threads[0], workerBodyA, nullptr);
    threadA = threads[0]; // Store reference to thread A for pinging
    printString("Thread A created\n");

    printString("Creating thread B...\n");
    thread_create(&threads[1], workerBodyB, nullptr);
    printString("Thread B created\n");

    printString("Creating thread C...\n");
    thread_create(&threads[2], workerBodyC, nullptr);
    printString("Thread C created\n");

    printString("All threads created. Waiting for completion...\n");
    printString("Thread C will ping thread A when it reaches iteration 5\n");

    while (!(finishedA && finishedB && finishedC)) {
        thread_dispatch();
    }

    printString("All threads completed!\n");
    printString("=== Ping Test Completed ===\n");
}