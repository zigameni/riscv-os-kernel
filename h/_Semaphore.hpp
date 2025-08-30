

#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include "list.hpp"
#include "tcb.hpp"        // Use TCB from the first implementation if appropriate
#include "memoryAllocator.hpp" // Custom memory allocator
// #include "syscall_c.hpp"

class _Semaphore {
public:
    explicit _Semaphore(unsigned init = 1);
    ~_Semaphore();

    int wait();
    int signal();
    int trywait();
    int close();

    // Static method to create a semaphore
    static _Semaphore* createSemaphore(unsigned init = 1);

    unsigned getValue() const { return value; }

    // Custom memory allocation
    void* operator new(size_t size);
    void* operator new[](size_t size);
    void operator delete(void *ptr);
    void operator delete[](void *ptr);

private:
    int value;
    bool closed;
    List<TCB> blockedList; // List of blocked threads

    void blockThread();   // Helper function to handle blocking
    void unblockThread(); // Helper function to handle unblocking

    // Priority absed methods
    // Add therad with priority
    void addThreadWithPriority(TCB* thread);

    // remove thread with heighst priority
    TCB* removeHighestPriorityThread();
};

#endif
