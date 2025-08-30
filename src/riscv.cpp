//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../test/printing.hpp"
// #include "../h/timer.hpp"

void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

using Body = void (*)(void*);

uint64 globalResult;

void Riscv::handleSupervisorTrap() {
    uint64 scause = r_scause();
    if (scause == 0x8 || scause == 0x9) { // Ecall from U-mode or S-mode
        uint64 volatile sepc = r_sepc() + 4; // Move to the next instruction
        uint64 volatile sstatus = r_sstatus();


        uint64 codeOperation = r_a0();
        uint64 result = handleOperation(codeOperation);
        storeToStack(result); // Save the result in a0
        w_sstatus(sstatus);
        w_sepc(sepc);

    }
    else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        uint64 volatile sepc1 = r_sepc();
        uint64 volatile sstatus1 = r_sstatus();
        handleTimerInterrupt();
        w_sstatus(sstatus1);
        w_sepc(sepc1);

    }
    else {
        // Log error details
        if (scause != 0) {
            printString("ERROR! SCAUSE: ");
            printInt(scause);
            printString(" at SEPC: ");
            // printInt(sepc);
            printString("\n");
        }
    }
    // w_sepc(sepc); // Ensure SEPC is updated regardless of operation
}

// declared in vector table
void Riscv::handleConsoleInterrupt() {
    console_handler();
}

// declared in vectorTable
void Riscv::handleTimerInterrupt() {
    uint64 volatile sepc1 = r_sepc();
    uint64 volatile sstatus1 = r_sstatus();

    extern void incrementTick();
    incrementTick();

    // Clear the supervisor software interrupt pending bit
    mc_sip(SIP_SSIP);

    // // Update sleeping threads
    TCB::updateSleepingThreads();

    w_sstatus(sstatus1);
    w_sepc(sepc1);
}

uint64 Riscv::handleOperation(uint64 codeOperation)
{
    uint64 returnValue = 0;
    _Semaphore* semHandlePtr;

    switch (codeOperation) {
        case SYSCALL_MEM_ALLOC: {
            // mem_alloc(size)
            size_t size;
            void* ptr;
            __asm__ volatile("mv %0, a1" : "=r" (size));
            ptr = MemoryAllocator::mem_alloc(size);
            returnValue = reinterpret_cast<uint64>(ptr); // Return allocated memory pointer
            break;
        }

        case SYSCALL_MEM_FREE: {
            // mem_free(ptr)
            void* memptr;
            __asm__ volatile("mv %0, a1" : "=r" (memptr));
            returnValue = MemoryAllocator::mem_free(memptr); // Return result of mem_free
            break;
        }

        // THREAD CASES
        case 0x09: {
            // thread_start
            TCB* tcbForStart;
            __asm__ volatile("mv %0, a1" : "=r" (tcbForStart));
            TCB::startThread(tcbForStart);
            break;
        }

        case 0x10: {
            // thread_create_without_start
            TCB** tcbNoStart;
            Body bodyNoStart;
            void* argNoStart;
            __asm__ volatile ("mv %0, a1" : "=r" (tcbNoStart));
            __asm__ volatile ("mv %0, a2" : "=r" (bodyNoStart));
            __asm__ volatile ("mv %0, a3" : "=r" (argNoStart));
            *tcbNoStart = TCB::createThreadWithoutStarting(bodyNoStart, argNoStart);
            returnValue = (*tcbNoStart != nullptr) ? 0 : -1;
            break;
        }

        case SYSCALL_THREAD_CREATE: {
            // thread_create
            TCB** tcb;
            Body body;
            void* arg;
            __asm__ volatile ("mv %0, a1" : "=r" (tcb));
            __asm__ volatile ("mv %0, a2" : "=r" (body));
            __asm__ volatile ("mv %0, a3" : "=r" (arg));
            *tcb = TCB::createThread(body, arg);
            returnValue = (*tcb != nullptr) ? 0 : -1;
            break;
        }

        case SYSCALL_THREAD_EXIT: {
            // thread_exit
            TCB::running->setFinished(true);
            TCB::dispatch();
            returnValue = 0;
            break;
        }

        case SYSCALL_THREAD_DISPATCH: {
            // thread_dispatch
            TCB::dispatch();
            break;
        }

        case 0x14: {
            // thread_join(handle)
            thread_t handle;
            __asm__ volatile ("mv %0, a1" : "=r" (handle));
            TCB::join(handle);
            TCB::dispatch();
            break;
        }

        case SYSCALL_GET_THREAD_ID: {
            // thread_join(handle)
            returnValue = TCB::running->getThreadId();
            TCB::dispatch();
            break;
        }

        case SYSCALL_PING: {
            // thread_join(handle)
            TCB* targetThread;
            targetThread = reinterpret_cast<TCB *>(Riscv::readA1());

            if(targetThread!= nullptr){
                targetThread->setPinged(true);
                returnValue =  0;
            }else {
                returnValue = -1;
            }

            break;
        }
        // SEMAPHORE CASES
        case SYSCALL_SEM_OPEN: {
            // sem_open
            unsigned init;
            _Semaphore** semHandle;
            __asm__ volatile ("mv %0, a2" : "=r" (init));
            __asm__ volatile ("mv %0, a1" : "=r" (semHandle));
            *semHandle = _Semaphore::createSemaphore(init);
            returnValue = (*semHandle != nullptr) ? 0 : -1;
            break;
        }

        case SYSCALL_SEM_CLOSE: {
            // sem_close
            __asm__ volatile ("mv %0, a1" : "=r" (semHandlePtr));
            if (semHandlePtr != nullptr)
                returnValue = semHandlePtr->close();
            else
                returnValue = -2;
            break;
        }

        case SYSCALL_SEM_WAIT: {
            // sem_wait
            __asm__ volatile ("mv %0, a1" : "=r" (semHandlePtr));
            if (semHandlePtr != nullptr)
                returnValue = semHandlePtr->wait();
            else
                returnValue = -2;
            break;
        }

        case SYSCALL_SEM_SIGNAL: {
            // sem_signal
            __asm__ volatile ("mv %0, a1" : "=r" (semHandlePtr));
            if (semHandlePtr != nullptr)
                returnValue = semHandlePtr->signal();
            else
                returnValue = -2;
            break;
        }

        case SYSCALL_SEM_TRYWAIT: {
            // sem_trywait
            __asm__ volatile ("mv %0, a1" : "=r" (semHandlePtr));
            if (semHandlePtr != nullptr)
                returnValue = semHandlePtr->trywait();
            else
                returnValue = -2;
            break;
        }

        case SYSCALL_TIME_SLEEP: {
            time_t time;
            __asm__ volatile ("mv %0, a1" : "=r" (time));
            if (time > 0) {
                returnValue = static_cast<uint64>(TCB::time_sleep(time));

            } else {
                returnValue = -1;
            }
            break;
        }
        // I/O CASES
        case 0x41: {
            // getc
            returnValue = __getc();
            break;
        }

        case 0x42: {
            // putc
            char c;
            __asm__ volatile ("mv %0, a1" : "=r" (c));
            __putc(c);
            break;
        }

        case 0x43: {
            TCB::dispatch();
            break;
        }

        default:
            break;
    }

    return returnValue;
}






