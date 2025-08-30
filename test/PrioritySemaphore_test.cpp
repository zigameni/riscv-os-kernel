#include "../h/syscall_c.hpp"
#include "PrioritySemaphore_test.hpp"
#include "printing.hpp"

static volatile bool allThreadsFinished = false;
static volatile int finishedThreadCount = 0;
static const int TOTAL_THREADS = 50;
static const int CRITICAL_SECTION_ENTRIES = 3;

static sem_t sharedSemaphore = nullptr;

// Function to simulate busy waiting proportional to thread ID
static void busyWait(uint64 threadId) {
    uint64 waitTime = threadId * 1000; // Proportional to thread ID
    for (uint64 i = 0; i < waitTime; i++) {
        // Busy wait
        __asm__ volatile("nop");
    }
}

static void workerThread(void* arg) {
    uint64 threadId = reinterpret_cast<uint64>(arg);

    printString("Thread ");
    printInt(threadId);
    printString(" starting...\n");

    for (int entry = 0; entry < CRITICAL_SECTION_ENTRIES; entry++) {
        printString("Thread ");
        printInt(threadId);
        printString(" waiting to enter critical section (entry ");
        printInt(entry + 1);
        printString("/");
        printInt(CRITICAL_SECTION_ENTRIES);
        printString(")\n");

        // Wait for semaphore (this will block if no resources available)
        sem_wait(sharedSemaphore);

        printString("Thread ");
        printInt(threadId);
        printString(" ENTERED critical section (entry ");
        printInt(entry + 1);
        printString("/");
        printInt(CRITICAL_SECTION_ENTRIES);
        printString(")\n");

        // Simulate work in critical section with busy waiting
        printString("Thread ");
        printInt(threadId);
        printString(" working in critical section...\n");
        busyWait(threadId);

        // Dispatch to allow other threads to run and validate priority ordering
        printString("Thread ");
        printInt(threadId);
        printString(" dispatching inside critical section...\n");
        thread_dispatch();

        printString("Thread ");
        printInt(threadId);
        printString(" EXITING critical section (entry ");
        printInt(entry + 1);
        printString("/");
        printInt(CRITICAL_SECTION_ENTRIES);
        printString(")\n");

        // Release the semaphore
        sem_signal(sharedSemaphore);

        // Small delay between entries
        time_sleep(5);
    }

    printString("Thread ");
    printInt(threadId);
    printString(" completed all critical section entries\n");

    // Increment finished count (simple increment, not atomic)
    finishedThreadCount++;

    // Check if all threads are finished
    if (finishedThreadCount >= TOTAL_THREADS) {
        allThreadsFinished = true;
    }
}

void PrioritySemaphore_test() {
    printString("=== Priority Semaphore Test ===\n");
    printString("Creating ");
    printInt(TOTAL_THREADS);
    printString(" threads competing for a semaphore with value 5\n");
    printString("Each thread will enter the critical section ");
    printInt(CRITICAL_SECTION_ENTRIES);
    printString(" times\n");
    printString("Priority is determined by thread ID (smaller ID = higher priority)\n");
    printString("Threads created first get smaller IDs and thus higher priority\n\n");

    // Create the shared semaphore with value 5
    if (sem_open(&sharedSemaphore, 5) != 0) {
        printString("Error: Failed to create semaphore\n");
        return;
    }

    printString("Shared semaphore created with value 5\n");
    printString("Creating threads...\n\n");

    thread_t threads[TOTAL_THREADS];

    // Create threads in order (0 to 49) - they will get IDs 1, 2, 3, ..., 50
    // Thread ID 1 = highest priority, Thread ID 50 = lowest priority
    for (int i = 0; i < TOTAL_THREADS; i++) {
        thread_create(&threads[i], workerThread, reinterpret_cast<void*>(i));
        printString("Created thread ");
        printInt(i);
        printString(" (will get ID ");
        printInt(i + 1);
        printString(")\n");
    }

    printString("\nAll threads created. Waiting for completion...\n");
    printString("Watch the output to see priority-based ordering in action!\n\n");

    // Wait for all threads to complete
    while (!allThreadsFinished) {
        thread_dispatch();
    }

    printString("\n=== All threads completed! ===\n");
    printString("Priority semaphore test finished successfully!\n");
    printString("The output should show that threads with smaller IDs (higher priority)\n");
    printString("tend to enter the critical section before threads with larger IDs.\n");
    printString("Since threads created first get smaller IDs, they have higher priority.\n");

    // Clean up
    sem_close(sharedSemaphore);
} 