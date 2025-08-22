//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP

#include "../lib/hw.h"

class Riscv
{
public:

    // pop sstatus.spp and sstatus.spie bits (has to be a non inline function)
    static void popSppSpie();



    // read register scause
    static uint64 r_scause();

    // write register scause
    static void w_scause(uint64 scause);

    // read register sepc
    static uint64 r_sepc();

    // write register sepc
    static void w_sepc(uint64 sepc);

    // read register stvec
    static uint64 r_stvec();

    // write register stvec
    static void w_stvec(uint64 stvec);

    // read register stval
    static uint64 r_stval();

    // write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    // mask set register sip
    static void ms_sip(uint64 mask);

    // mask clear register sip
    static void mc_sip(uint64 mask);

    // read register sip
    static uint64 r_sip();

    // write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus
    {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    // mask set register sstatus
    static void ms_sstatus(uint64 mask);

    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    // read register sstatus
    static uint64 r_sstatus();

    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    // supervisor trap
    static void supervisorTrap();

    //stvec Vector "table"
    static void stvecVectorTable();

    //read a0 register
    static uint64 r_a0();

    //write to a0 register
    static void w_a0(uint64 writeValue);

    static void storeToStack(uint64 value);

    // Write functions
    static void writeA0(uint64 value);
    static void writeA1(uint64 value);
    static void writeA2(uint64 value);
    static void writeA3(uint64 value);
    static void writeA4(uint64 value);
    static void writeA5(uint64 value);
    static void writeA6(uint64 value);
    static void writeA7(uint64 value);

    // Read functions
    static uint64 readA0();
    static uint64 readA1();
    static uint64 readA2();
    static uint64 readA3();
    static uint64 readA4();
    static uint64 readA5();
    static uint64 readA6();
    static uint64 readA7();
    
    // // general function to read from a register.
    // static uint64 readRegister(const char* registerName);
    //
    // // General function to write into a register
    // static void writeToRegister(const char* registerName, uint64 value);

private:

    // static uint64 systemCalls(uint64 *arguments);

    // supervisor trap handler
    static void handleSupervisorTrap();

    static void handleTimerInterrupt();

    static void handleConsoleInterrupt();


    // handle all system calls
    uint64 static handleOperation(uint64 codeOperation);

};


// inline  uint64 readRegister(const char* registerName) {
//     uint64 value;
//     __asm__ volatile("mv %0, %s" : "=r"(value) : "s"(registerName));
//     return value;
// }
//
// inline void Riscv::writeToRegister(const char* registerName, uint64 value) {
//     __asm__ volatile("mv %1s, %0" : : "s"(registerName), "r"(value));
// }

inline void Riscv::storeToStack(uint64 value) {
    __asm__ volatile("sw %0, 80(fp)" : : "r"(value));
}

inline uint64 Riscv::r_scause()
{
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_sepc()
{
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc)
{
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec()
{
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec)
{
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_stval()
{
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval)
{
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::ms_sip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::ms_sstatus(uint64 mask)
{
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask)
{
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sstatus()
{
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus)
{
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

//a0
inline uint64 Riscv::r_a0()
{
    uint64 volatile a0;
    __asm__ volatile ("mv %0, a0" : "=r"(a0));
    return a0;
}


inline void Riscv::w_a0(uint64 writeValue) {
    __asm__ volatile ("mv a0, %0" : : "r"(writeValue));
}

// Write functions
inline void Riscv::writeA0(uint64 value) {
    __asm__ volatile("mv a0, %0" : : "r"(value));
}

inline void Riscv::writeA1(uint64 value) {
    __asm__ volatile("mv a1, %0" : : "r"(value));
}

inline void Riscv::writeA2(uint64 value) {
    __asm__ volatile("mv a2, %0" : : "r"(value));
}

inline void Riscv::writeA3(uint64 value) {
    __asm__ volatile("mv a3, %0" : : "r"(value));
}

inline void Riscv::writeA4(uint64 value) {
    __asm__ volatile("mv a4, %0" : : "r"(value));
}

inline void Riscv::writeA5(uint64 value) {
    __asm__ volatile("mv a5, %0" : : "r"(value));
}

inline void Riscv::writeA6(uint64 value) {
    __asm__ volatile("mv a6, %0" : : "r"(value));
}

inline void Riscv::writeA7(uint64 value) {
    __asm__ volatile("mv a7, %0" : : "r"(value));
}

// Read functions
inline uint64 Riscv::readA0() {
    uint64 value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}
inline uint64 Riscv::readA1() {
    uint64 value;
    __asm__ volatile("mv %0, a1" : "=r"(value));
    return value;
}

inline uint64 Riscv::readA2() {
    uint64 value;
    __asm__ volatile("mv %0, a2" : "=r"(value));
    return value;
}

inline uint64 Riscv::readA3() {
    uint64 value;
    __asm__ volatile("mv %0, a3" : "=r"(value));
    return value;
}

inline uint64 Riscv::readA4() {
    uint64 value;
    __asm__ volatile("mv %0, a4" : "=r"(value));
    return value;
}

inline uint64 Riscv::readA5() {
    uint64 value;
    __asm__ volatile("mv %0, a5" : "=r"(value));
    return value;
}

inline uint64 Riscv::readA6() {
    uint64 value;
    __asm__ volatile("mv %0, a6" : "=r"(value));
    return value;
}

inline uint64 Riscv::readA7() {
    uint64 value;
    __asm__ volatile("mv %0, a7" : "=r"(value));
    return value;
}


#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP
