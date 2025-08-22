// timer.hpp
#ifndef TIMER_HPP
#define TIMER_HPP
#include "../lib/hw.h"
extern volatile int tickCount; // Volatile to prevent optimization issues

void incrementTick(); // Function to be called on each timer interrupt
time_t getCurrentTick(); // Function to retrieve the current tick count

#endif // TIMER_HPP
