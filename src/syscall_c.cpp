#include "../h/syscall_c.hpp"
#include "../h/tcb.hpp"
#include "../h/_Semaphore.hpp"
#include "../h/riscv.hpp"
// #include "../h/timer.hpp"

void* mem_alloc(size_t size) {
    size_t alloc_size;
    alloc_size = (size % MEM_BLOCK_SIZE != 0) ? ((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE : size;
    Riscv::writeA1(alloc_size);
    Riscv::writeA0(SYSCALL_MEM_ALLOC);
    __asm__ volatile("ecall");
    void* returnValue = reinterpret_cast<void *>(Riscv::readA0());
    return returnValue;
}

int mem_free (void* ptr) {
    Riscv::writeA1(reinterpret_cast<uint64>(ptr));
    Riscv::writeA0(SYSCALL_MEM_FREE);
    __asm__ volatile("ecall");
    uint64 returnValue = Riscv::readA0();
    return (int)returnValue;
}

void thread_start(TCB* tcb) {
    Riscv::writeA1(reinterpret_cast<uint64>(tcb));
    Riscv::writeA0(0x09);
    __asm__ volatile("ecall");
}

int thread_create_without_start(thread_t* handle,
                                void (*start_routine)(void*), void* arg) {

    Riscv::writeA3(reinterpret_cast<uint64>(arg));
    Riscv::writeA2(reinterpret_cast<uint64>(start_routine));
    Riscv::writeA1(reinterpret_cast<uint64>(handle));
    Riscv::writeA0(0x10);
    __asm__ volatile("ecall");
    uint64 returnValue= Riscv::readA0();
    return (int)returnValue;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg) {
    Riscv::writeA3(reinterpret_cast<uint64>(arg));
    Riscv::writeA2(reinterpret_cast<uint64>(start_routine));
    Riscv::writeA1(reinterpret_cast<uint64>(handle));
    Riscv::writeA0(SYSCALL_THREAD_CREATE);
    __asm__ volatile("ecall");
    uint64 returnValue= Riscv::readA0();
    return (int)returnValue;
}

int thread_exit() {
    Riscv::w_a0(0x12);
    Riscv::writeA0(SYSCALL_THREAD_EXIT);
    __asm__ volatile ("ecall");
    uint64 returnValue= Riscv::readA0();
    return (int)returnValue;
}

void thread_dispatch() {
    Riscv::writeA0(SYSCALL_THREAD_DISPATCH);
    __asm__ volatile ("ecall");
}

void thread_join(thread_t handle) {
    Riscv::writeA1(reinterpret_cast<uint64>(handle));
    Riscv::writeA0(0x14);
    __asm__ volatile ("ecall");
}




int sem_open(sem_t* handle, unsigned init) {
    Riscv::writeA2(init);
    Riscv::writeA1(reinterpret_cast<uint64>(handle));
    Riscv::writeA0(SYSCALL_SEM_OPEN);
    __asm__ volatile ("ecall");
    uint64 returnValue = Riscv::readA0();
    return (int)returnValue;
}

int sem_close(sem_t handle) {
    Riscv::writeA1(reinterpret_cast<uint64>(handle));
    Riscv::writeA0(SYSCALL_SEM_CLOSE);
    __asm__ volatile ("ecall");
    uint64 returnValue = Riscv::readA0();
    return (int)returnValue;
}

int sem_wait(sem_t id) {
    Riscv::writeA1(reinterpret_cast<uint64>(id));
    Riscv::writeA0(SYSCALL_SEM_WAIT);
    __asm__ volatile ("ecall");
    uint64 returnValue = Riscv::readA0();
    return (int)returnValue;
}

int sem_signal(sem_t id) {
    Riscv::writeA1(reinterpret_cast<uint64>(id));
    Riscv::writeA0(SYSCALL_SEM_SIGNAL);
    __asm__ volatile ("ecall");
    uint64 returnValue = Riscv::readA0();
    return (int)returnValue;
}

int sem_trywait(sem_t id) {
    Riscv::writeA1(reinterpret_cast<uint64>(id));
    Riscv::writeA0(SYSCALL_SEM_TRYWAIT);
    __asm__ volatile ("ecall");
    uint64 returnValue = Riscv::readA0();
    return (int)returnValue;
}

char getc() {
    Riscv::writeA0(SYSCALL_GETC);
    __asm__ volatile ("ecall");
    char returnValue = (char)Riscv::readA0();
    return returnValue;
}

void putc(char c) {
    Riscv::writeA1(c);
    Riscv::writeA0(SYSCALL_PUTC);
    __asm__ volatile ("ecall");
}

int time_sleep(time_t time) {
    Riscv::writeA1(time);
    Riscv::writeA0(SYSCALL_TIME_SLEEP);
    __asm__ volatile ("ecall");
    uint64 returnValue = Riscv::readA0();
    return (int)returnValue;
}
