#include "../h/_Semaphore.hpp"
#include "../h/syscall_c.hpp"

// int _Semaphore::wait() {
//     if(this->closed) {
//         return -1;
//     }
//
//     if((int)--this->value < 0) {
//         this->blocked.addLast(TCB::running);
//         TCB::running->setBlocked(true);
//         thread_dispatch();
//
//         //Check if it was closed with close() or it was signaled with signal()
//         if(this->closed) {
//             return -1;
//         }
//     }
//     return 0;
// }
//
// int _Semaphore::signal() {
//     if(this->closed) {
//         return -1;
//     }
//     if((int)++this->value <= 0) {
//         TCB* tcb;
//         tcb = this->blocked.removeFirst();
//         tcb->setBlocked(false);
//         Scheduler::put(tcb);
//     }
//     return 0;
// }
//
// _Semaphore * _Semaphore::createSemaphore(unsigned int init) {
//     return new _Semaphore(init);
// }
//
// int _Semaphore::close() {
//     if (closed) {
//         return -1;
//     }
//     closed = true;
//
//     while (this->blocked.peekFirst()) {
//         TCB* tcb = this->blocked.removeFirst();
//         tcb->setBlocked(false);
//         Scheduler::put(tcb);
//     }
//     return 0;
// }


// Implementation of the _Semaphore methods

_Semaphore::_Semaphore(unsigned init) : value(init), closed(false) {
    // blockedList.head = blockedList.tail = nullptr;
}

_Semaphore::~_Semaphore() {
    close(); // Ensure all resources are freed
}

int _Semaphore::wait() {
    if (closed) return -1;

    // Atomically decrement value (if implemented elsewhere)
    if (--value < 0) {
        blockThread();
        if (closed) return -1; // Check if semaphore was closed while blocked
    }

    return 0; // Success
}

int _Semaphore::signal() {
    if (closed) return -1;

    // Atomically increment value (if implemented elsewhere)
    if (++value <= 0) {
        unblockThread();
    }

    return 0; // Success
}

int _Semaphore::trywait() {
    if (closed) return -1; // Return error if semaphore is closed

    if (value > 0) {
        --value; // Decrement the semaphore
        return 1; // Success (decremented without blocking)
    }

    return 0; // Semaphore is locked (value <= 0)
}
//

int _Semaphore::close() {
    if (closed) return -1;

    closed = true;
    // Unblock all waiting threads
    while (TCB* tcb = blockedList.removeFirst()) {
        tcb->setBlocked(false);
        Scheduler::put(tcb);
    }

    return 0; // Success
}

_Semaphore* _Semaphore::createSemaphore(unsigned init) {
    return new _Semaphore(init);
}

void* _Semaphore::operator new(size_t size) {
    return MemoryAllocator::mem_alloc(size);
}

void* _Semaphore::operator new[](size_t size) {
    return MemoryAllocator::mem_alloc(size);
}

void _Semaphore::operator delete(void *ptr) {
    MemoryAllocator::mem_free(ptr);
}

void _Semaphore::operator delete[](void *ptr) {
    MemoryAllocator::mem_free(ptr);
}

void _Semaphore::blockThread() {
    blockedList.addLast(TCB::running); // Add current thread to blocked list
    TCB::running->setBlocked(true);    // Mark the thread as blocked
    thread_dispatch();                // Yield the CPU
}

void _Semaphore::unblockThread() {
    if (TCB* tcb = blockedList.removeFirst()) {
        tcb->setBlocked(false);
        Scheduler::put(tcb); // Add the unblocked thread back to the scheduler
    }
}
