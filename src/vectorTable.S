# 1 "src/vectorTable.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/vectorTable.S"
.extern _ZN5Riscv20handleSupervisorTrapEv
.extern _ZN5Riscv20handleTimerInterruptEv
.extern _ZN5Riscv22handleConsoleInterruptEv

.align 4
.global _ZN5Riscv16stvecVectorTableEv
.type _ZN5Riscv16stvecVectorTableEv, @function

# Vector table
_ZN5Riscv16stvecVectorTableEv:
    j softwareInterruptHandler
    j timerInterruptHandler
    .skip 28
    j consoleHandler
    sret

# Macro to save registers to the stack
.macro SAVE_REGISTERS
    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        sd x\index, \index * 8(sp)
    .endr
.endm

# Macro to restore registers from the stack
.macro RESTORE_REGISTERS
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256
.endm

# Software interrupt handler
softwareInterruptHandler:
    SAVE_REGISTERS
    call _ZN5Riscv20handleSupervisorTrapEv
    RESTORE_REGISTERS
    sret

# Timer interrupt handler
timerInterruptHandler:
    SAVE_REGISTERS
    call _ZN5Riscv20handleTimerInterruptEv
    RESTORE_REGISTERS
    sret

# Console interrupt handler
consoleHandler:
    SAVE_REGISTERS
    call _ZN5Riscv22handleConsoleInterruptEv
    RESTORE_REGISTERS
    sret
