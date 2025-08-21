//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"

void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

// Interrupt handler, its called from the `supervisorTrap.S`
void Riscv::handleSupervisorTrap()
{

    // RISCV trap cause codes
    const uint64 ECALL_FROM_USER_MODE = 0x0000000000000008UL;
    const uint64 ECALL_FROM_SUPERVISOR_MODE = 0x0000000000000009UL;
    const uint64 SUPERVISOR_SOFTWARE_INTERRUPT = 0x8000000000000001UL;
    const uint64 SUPERVISOR_EXTERNAL_INTERRUPT = 0x8000000000000009UL;


    // Read the scause register to determine what caused the supervisor trap
    // scause contains the trap cause code that tells us whether this was an
    // interrupt or exception and what specific type it was
    uint64 scause = r_scause();

    // Handle system calls (environment calls from user or supervisor mode)
    if (scause == ECALL_FROM_USER_MODE || scause == ECALL_FROM_SUPERVISOR_MODE)
    {
        // scause = 0x08: Environment call from User mode (ecall instruction)
        // scause = 0x09: Environment call from Supervisor mode (ecall instruction)
        // These are synchronous exceptions, not interrupts (MSB = 0)

        // Save the exception program counter and advance it by 4 bytes
        // We add 4 to skip over the ecall instruction that caused the trap
        // so when we return, we don't re-execute the same ecall
        uint64 volatile sepc = r_sepc() + 4;

        // Save the current supervisor status register
        // This preserves the processor state (interrupt enable bits, privilege mode, etc.)
        uint64 volatile sstatus = r_sstatus();

        // Reset the time slice counter to 0
        // System calls trigger immediate context switch, so current thread
        // gets a fresh time slice when it eventually runs again
        TCB::timeSliceCounter = 0;

        // Perform context switch to another thread
        // This saves current thread's context and loads next thread's context
        TCB::dispatch();

        // Restore the status register for the newly scheduled thread
        // This sets up the processor state for the new thread
        w_sstatus(sstatus);

        // Set the return address for the newly scheduled thread
        // When sret is executed, execution will resume at this address
        w_sepc(sepc);
    }
    // Handle timer interrupts for preemptive scheduling
    else if (scause == SUPERVISOR_SOFTWARE_INTERRUPT)
    {
        // scause = 0x8000000000000001: Supervisor software interrupt
        // This is an asynchronous interrupt (MSB = 1) typically triggered by
        // machine timer interrupt forwarded by CLINT (Core Local Interrupt Controller)

        // Clear the supervisor software interrupt pending bit
        // This acknowledges the interrupt and prevents it from immediately
        // triggering again when we re-enable interrupts
        mc_sip(SIP_SSIP);

        // Increment the time slice counter
        // Each timer interrupt represents one "tick" of execution time
        // for the currently running thread
        TCB::timeSliceCounter++;

        // Check if current thread has used up its allocated time slice
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            // Time slice expired - perform preemptive context switch

            // Save current program counter (no +4 needed since this is an interrupt,
            // not an exception - we want to resume exactly where we were interrupted)
            uint64 volatile sepc = r_sepc();

            // Save current processor status
            uint64 volatile sstatus = r_sstatus();

            // Reset time slice counter for round-robin scheduling
            TCB::timeSliceCounter = 0;

            // Switch to next thread in the ready queue
            // Current thread goes back to ready state, next thread becomes running
            TCB::dispatch();

            // Restore processor state for the newly scheduled thread
            w_sstatus(sstatus);

            // Set return address for the newly scheduled thread
            w_sepc(sepc);
        }
        // If time slice hasn't expired, simply return to the current thread
        // and let it continue executing
    }
    // Handle external interrupts (I/O devices)
    else if (scause == SUPERVISOR_EXTERNAL_INTERRUPT)
    {
        // scause = 0x8000000000000009: Supervisor external interrupt
        // This is an asynchronous interrupt (MSB = 1) from external devices
        // connected through PLIC (Platform-Level Interrupt Controller)
        // Common sources: keyboard, mouse, network, disk, etc.

        // Handle console/keyboard input
        // This function will read from the console device and potentially
        // wake up threads waiting for input
        console_handler();

        // Note: No context switch occurs here unless console_handler()
        // explicitly triggers one. External interrupts are handled quickly
        // and control returns to the same thread that was interrupted.
    }
    else
    {
        // Unexpected or unhandled trap cause
        // This could be:
        // - Illegal instruction exception
        // - Load/store access fault
        // - Page fault (if virtual memory is implemented)
        // - Breakpoint exception
        // - Other hardware exceptions
        //
        // In a production OS, this would typically:
        // - Log the error
        // - Terminate the offending process
        // - Possibly panic if it's a kernel-level fault

        // Currently no action is taken - this could lead to undefined behavior
        // in case of unexpected traps
    }

    // Function returns and the trap handler wrapper will execute 'sret'
    // instruction to return from supervisor mode back to the interrupted context
}