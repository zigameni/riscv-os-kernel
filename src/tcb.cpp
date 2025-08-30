//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../test/printing.hpp"
#include "../h/syscall_c.hpp"
#include "../h/timer.hpp"


TCB *TCB::running = nullptr;
// uint64 TCB::timeSliceCounter = 0;

int TCB::maxThreads = 5;
int TCB::activeThreadCount = 0;
List<TCB> TCB::blockedThreads;

void TCB::setMaximumThreads(int num_of_threads) {
    if(num_of_threads>0){
        maxThreads = num_of_threads;
        printString("Maximum threads set to: ");
        printInt(maxThreads);
        printString("\n");
    }else {
        printString("Error: Invalid number of threads. Must be postive.");
    }
}


TCB *TCB::createThread(Body body, void* arg)
{
    TCB* newTCB = new TCB(body, arg);
    newTCB->setStartTime(getCurrentTick());

    // check if we can crete more threads
    if(activeThreadCount< maxThreads){
        // we can create the thread normally
        activeThreadCount++;

        Scheduler::put(newTCB);

    }else {
        newTCB->setBlocked(true);
        blockedThreads.addLast(newTCB);
    }


//    Scheduler::put(newTCB);
    // If this is not the main thread, dispatch it immediately
    if (body != nullptr) {
        dispatch();
    }
    return newTCB;
    // return newTCB;
}

TCB *TCB::createThreadWithoutStarting(Body body, void* arg)
{
    TCB* newTCB = new TCB(body, arg);
    //Scheduler::put(newTCB);
    return newTCB;
}

void TCB::yield()
{
    Riscv::w_a0(0x13);
    __asm__ volatile ("ecall");
}

void TCB::dispatch()
{
    TCB *old = running;


    // Check if thread was pinged
    if(old != nullptr && old->isPinged() && !old->isPingedReported()){
        // calculate execution time
        uint64 currentTime = getCurrentTick();
        uint64 executionTime = currentTime- old->getStartTime();

        printString("=== PING REPORT ===\n");
        printString("Thread ID: ");
        printInt(old->getThreadId());
        printString("\n");
        printString("Memory blocks allocated: ");
        printInt(old->getMemoryBlocks());
        printString("\n");
        printString("Total execution time: ");
        printInt(executionTime);
        printString(" ticks\n");
        printString("==================\n");

        old->setPingedReported(true);
    }


    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();

    if(running->isMain()) {
        Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    }
    else {
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    }
    TCB::contextSwitch(&old->context, &running->context);


        // TCB *old = running;
        // if (!old->isFinished() && !old->isBlocked()) {
        //     Scheduler::put(old);
        // }
        //
        // // Keep getting threads from the scheduler until a non-blocked, non-sleeping thread is found
        // while (true) {
        //     running = Scheduler::get();
        //     if (!running->isBlocked() && !running->isSleeping()) {
        //         break; // Found a suitable thread
        //     }
        //
        //     // If the thread is blocked or sleeping, put it back and try again
        //     Scheduler::put(running);
        //     updateSleepingThreads(); // Update sleeping threads before trying again
        // }
        //
        // if (running->isMain()) {
        //     Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
        // } else {
        //     Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
        // }
        //
        // TCB::contextSwitch(&old->context, &running->context);
    }




void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);

    // decrese the thread count, and try to unblock a waiting thread.
    activeThreadCount--;

    if(blockedThreads.peekFirst() && activeThreadCount < maxThreads){
        TCB* blockedThread = blockedThreads.removeFirst();
        activeThreadCount++;
        Scheduler::put(blockedThread);

    }



    running->releaseAll();
    thread_dispatch();
}

void TCB::join(TCB* handle) {
    if(!handle->isFinished()) {
        running->setBlocked(true);
        handle->joined.addLast(running);
    }
}

void TCB::releaseAll() {
    while (this->joined.peekFirst()) {
        TCB* tcb = this->joined.removeFirst();
        tcb->setBlocked(false);
        timeSleepCounter=0;
        Scheduler::put(tcb);
    }
}

// int TCB::time_sleep(time_t time) {
//     // if (time <= 0) return -1; // Invalid sleep time
//     //
//     // // Mark the current thread as blocked
//     // TCB::running->setBlocked(true);
//     // time_t startTicks = getCurrentTick(); // Get the current ticks
//     //
//     // // Loop until the specified sleep time has elapsed
//     // while (true) {
//     //     time_t elapsedTicks = getCurrentTick() - startTicks; // Calculate elapsed time
//     //     if (elapsedTicks >= time) {
//     //         TCB::running->setBlocked(false); // Unblock the thread
//     //         return 0; // Success
//     //     }
//     //     thread_dispatch(); // Allow other threads to execute
//     // }
// }
List<TCB> TCB::sleepingThreads;

int TCB::time_sleep(time_t time) {
    if (time <= 0) return -1 ;
    if (running->isMain()) return -1;

    running->timeSleepCounter = time;
    running->setBlocked(true);
    sleepingThreads.addLast(running);

    // Switch to the next thread
    dispatch();

    return 0;
}

void TCB::updateSleepingThreads() {
    bool dispatchNeeded = false;
    TCB* tcb = sleepingThreads.peekFirst();
    while (tcb != nullptr) {

        tcb->timeSleepCounter--;
        if (tcb->timeSleepCounter <= 0) {
            sleepingThreads.removeFirst();
            tcb->setBlocked(false);
            Scheduler::put(tcb);
            tcb = sleepingThreads.peekFirst();
            dispatchNeeded = true;
            // dispatch();
        } else {
            break;  // As the list is sorted by wake-up time, we can stop here
        }
    }
    if(dispatchNeeded)dispatch();

}

static uint64 nextThreadId = 1;
uint64 TCB::getNextThreadId() {
    return nextThreadId++;
}


