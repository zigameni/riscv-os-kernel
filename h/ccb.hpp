//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_CCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_CCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

// Coroutine Control Block
class CCB
{
public:
    ~CCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    // Pokazivac na jednu funkciju
    using Body = void (*)();

    static CCB *createCoroutine(Body body);

    static void yield();//  koja poziva korutine da predaju procesor.

    static CCB *running;

private:
    explicit CCB(Body body) :
            body(body),
            stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
            context({body != nullptr ? (uint64) body : 0,
                     stack != nullptr ? (uint64) &stack[STACK_SIZE] : 0
                    }),
            finished(false)
    {
        if (body != nullptr) { Scheduler::put(this); }
    }

    struct Context
    {
        uint64 ra; // gde treba da se vrati, dokle je stiglo
        uint64 sp; // stack pointer
    };

    Body body;
    uint64 *stack;// pokazivac na niz lokacija u memoriju koji koristimo kao stek
    Context context;
    bool finished;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 constexpr STACK_SIZE = 1024;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_CCB_HPP
