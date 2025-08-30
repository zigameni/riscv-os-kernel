//
// Created by os on 8/29/25.
//

#include "Resource_test.hpp"
#include "../h/Resource.hpp"
#include "../h/syscall_c.hpp"
#include "printing.hpp"

static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;

static Resource* sharedResource = nullptr;

static void workerBodyA(void* arg) {
    printString("Thread A starting...\n");

    // Try to take 3 instances
    printString("Thread A requesting 3 instances...\n");
    sharedResource->take(3);
    printString("Thread A acquired 3 instances\n");

    // Simulate work
    for (int i = 0; i < 3; i++) {
        printString("Thread A working with instance ");
        printInt(i + 1);
        printString("\n");
        time_sleep(20); // Sleep for a bit
    }

    // Return 2 instances
    printString("Thread A returning 2 instances...\n");
    sharedResource->give_back(2);

    // Simulate more work with remaining instance
    printString("Thread A working with remaining instance\n");
    time_sleep(30);

    // Return the last instance
    printString("Thread A returning last instance...\n");
    sharedResource->give_back(1);

    printString("Thread A finished!\n");
    finishedA = true;
}

static void workerBodyB(void* arg) {
    printString("Thread B starting...\n");

    // Wait a bit before requesting resources
    time_sleep(10);

    // Try to take 2 instances
    printString("Thread B requesting 2 instances...\n");
    sharedResource->take(2);
    printString("Thread B acquired 2 instances\n");

    // Simulate work
    for (int i = 0; i < 2; i++) {
        printString("Thread B working with instance ");
        printInt(i + 1);
        printString("\n");
        time_sleep(25);
    }

    // Return both instances
    printString("Thread B returning 2 instances...\n");
    sharedResource->give_back(2);

    printString("Thread B finished!\n");
    finishedB = true;
}

static void workerBodyC(void* arg) {
    printString("Thread C starting...\n");

    // Wait a bit more before requesting resources
    time_sleep(20);

    // Try to take 4 instances (this should block until enough are available)
    printString("Thread C requesting 4 instances...\n");
    sharedResource->take(4);
    printString("Thread C acquired 4 instances\n");

    // Simulate work
    for (int i = 0; i < 4; i++) {
        printString("Thread C working with instance ");
        printInt(i + 1);
        printString("\n");
        time_sleep(15);
    }

    // Return all instances
    printString("Thread C returning 4 instances...\n");
    sharedResource->give_back(4);

    printString("Thread C finished!\n");
    finishedC = true;
}

void Resource_test() {
    printString("=== Resource Management Test ===\n");

    // Create a resource with 5 instances
    sharedResource = new Resource(5);

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
    printString("=== Resource Test Completed ===\n");

    // Clean up
    delete sharedResource;
}