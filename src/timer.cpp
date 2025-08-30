#include "../h/timer.hpp"
#include "../h/syscall_c.hpp"

volatile int tickCount = 0;

void incrementTick() {
    tickCount++;
}

time_t getCurrentTick() {
    return tickCount;
}

// int time_sleep_tcb(time_t time) {
//     if (time <= 0) return -1; // Return error if time is non-positive
//
//     TCB::running->setBlocked(true);
//     int startTicks = getCurrentTick();
//
//     while (true) {
//         int elapsedTicks = getCurrentTick() - startTicks;
//         if (elapsedTicks >= time) {
//             TCB::running->setBlocked(false);
//             return 0; // Success
//         }
//         thread_dispatch(); // Allow other threads to run
//     }
// }
