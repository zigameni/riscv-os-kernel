// System call numbers as macros

#define SYSCALL_MEM_ALLOC 0x01  // Allocate memory
#define SYSCALL_MEM_FREE  0x02  // Free memory
#define SYSCALL_THREAD_CREATE 0x11 // Create a thread
#define SYSCALL_THREAD_EXIT  0x12  // Exit a thread
#define SYSCALL_THREAD_DISPATCH 0x13 // Dispatch a thread
#define SYSCALL_SEM_OPEN   0x21  // Open a semaphore
#define SYSCALL_SEM_CLOSE  0x22  // Close a semaphore
#define SYSCALL_SEM_WAIT   0x23  // Wait on a semaphore
#define SYSCALL_SEM_SIGNAL 0x24  // Signal a semaphore
#define SYSCALL_SEM_TIMEDWAIT 0x25 // Timed wait on a semaphore
#define SYSCALL_SEM_TRYWAIT  0x26  // Try wait on a semaphore
#define SYSCALL_TIME_SLEEP  0x31  // Sleep for a specified time
#define SYSCALL_GETC        0x41  // Get a character from the console
#define SYSCALL_PUTC        0x42  // Put a character to the console