
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"


Semaphore::Semaphore(unsigned int init) {
    sem_open(&this->myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(this->myHandle);
}

int Semaphore::wait() {
    return sem_wait(this->myHandle);
}

int Semaphore::signal() {
    return sem_signal(this->myHandle);
}



// Implementation of static methods

void Thread::dispatch() {
    thread_dispatch();
}

Thread::Thread(void (*body)(void*), void* arg) {
    thread_create(&this->myHandle, runWrapper, this);
}



Thread::~Thread() {
    //thread_exit(); // If necessary, use thread_exit() to terminate the thread
    delete this->myHandle;
}

int Thread::start() {
    thread_start(this->myHandle);
    return 0;
}

Thread::Thread() {
    thread_create_without_start(&this->myHandle, runWrapper, this);
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}



void Console::putc(char c) {
    syscall_c::putc(c);
}

char Console::getc() {
    return syscall_c::getc();
}
