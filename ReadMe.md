# RISC-V Operating System Kernel

[![RISC-V](https://img.shields.io/badge/Architecture-RISC--V-green.svg)](https://riscv.org/)
[![C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Assembly](https://img.shields.io/badge/Assembly-RISC--V%20Assembly-orange.svg)](https://riscv.org/)
[![OS](https://img.shields.io/badge/Type-Operating%20System-red.svg)](https://en.wikipedia.org/wiki/Operating_system)

## 📖 Introduction / Uvod

**English:**  
This project implements a small but fully functional kernel of a multithreaded operating system with time sharing capabilities. The kernel provides memory allocation, thread management, semaphores, and time-sharing support, along with asynchronous context switching and preemption on timer and keyboard interrupts.

**Serbian:**  
Cilj ovog projekta jeste realizacija malog, ali sasvim funkcionalnog jezgra (engl. kernel) operativnog sistema koji podržava niti (engl. multithreaded operating system) sa deljenjem vremena (engl. time sharing). Jezgro obezbeđuje alokator memorije i upravljanje nitima, koncept niti, semafore i podršku deljenju vremena, kao i asinhronu promenu konteksta i preotimanje (engl. preemption) na prekid od tajmera i od tastature.

## 🎯 Project Goals / Ciljevi projekta

- **Memory Allocator** - Dynamic memory management system
- **Thread Management** - Lightweight process (thread) creation and scheduling
- **Semaphores** - Synchronization primitives for thread coordination
- **Time Sharing** - Cooperative and preemptive multitasking
- **Interrupt Handling** - Timer and keyboard interrupt support
- **Context Switching** - Efficient thread context management

## 🏗️ Architecture / Arhitektura

### System Design
The kernel is implemented as a "library" kernel where the user application and kernel share the same address space, representing a statically linked single program pre-loaded into computer memory. Concurrent processes created within the application are actually lightweight processes (threads) running within that program.

### Target Platform
- **Processor Architecture**: RISC-V
- **Implementation Languages**: C/C++ and RISC-V Assembly
- **Execution Environment**: RISC-V processor emulator (virtual environment)
- **System Type**: Embedded system configuration

## 🚀 Features / Funkcionalnosti

### Core System Features
- ✅ **Memory Management**: Custom memory allocator with block splitting and coalescing
- ✅ **Thread System**: Lightweight thread creation, scheduling, and management
- ✅ **Synchronization**: Semaphore implementation for thread coordination
- ✅ **Interrupt Handling**: Timer and keyboard interrupt support
- ✅ **Context Switching**: Efficient thread context management
- ✅ **Time Sharing**: Cooperative multitasking with preemption support

### Extended Features
- 🔧 **getThreadId()**: System call to retrieve current thread ID
- 🔧 **ping()**: Thread monitoring system with memory and execution time tracking
- 🔧 **SetMaximumThreads()**: Dynamic thread limit management
- 🔧 **Resource Class**: Thread-safe resource management with semaphores
- 🔧 **Priority Semaphores**: Priority-based thread scheduling
- 🔧 **Memory Tracking**: Per-thread memory allocation monitoring

## 📁 Project Structure / Struktura projekta

```
project-base-v1.1/
├── h/                          # Header files
│   ├── tcb.hpp                # Thread Control Block definitions
│   ├── riscv.hpp              # RISC-V architecture interface
│   ├── syscall_c.hpp          # System call C API
│   ├── _Semaphore.hpp         # Semaphore implementation
│   ├── Resource.hpp           # Resource management class
│   ├── memoryAllocator.hpp    # Memory management system
│   ├── scheduler.hpp          # Thread scheduler
│   ├── constants.hpp          # System constants and definitions
│   └── ...
├── src/                        # Source files
│   ├── main.cpp               # Main entry point
│   ├── tcb.cpp                # Thread Control Block implementation
│   ├── riscv.cpp              # RISC-V system implementation
│   ├── syscall_c.cpp          # System call implementations
│   ├── _Semaphore.cpp         # Semaphore implementation
│   ├── Resource.cpp            # Resource class implementation
│   ├── memoryAllocator.cpp    # Memory allocator implementation
│   ├── vectorTable.S          # RISC-V interrupt vector table
│   └── ...
├── test/                       # Test files
│   ├── userMain.cpp           # Main test program
│   ├── GetThreadId_test.cpp   # Thread ID system call test
│   ├── Resource_test.cpp      # Resource management test
│   ├── PrioritySemaphore_test.cpp  # Priority semaphore test
│   ├── SetMaximumThreads_test.cpp  # Thread limit test
│   └── ...
├── lib/                        # Library files
│   ├── hw.h                   # Hardware definitions
│   ├── console.h              # Console interface
│   └── ...
├── kernel.ld                   # Linker script
├── Makefile                    # Build configuration
└── README.md                   # This file
```

## 🛠️ Building and Running / Kompajliranje i pokretanje

### Prerequisites / Preduslovi
- RISC-V toolchain (GCC, Binutils)
- QEMU RISC-V emulator
- Make build system

### Build Commands / Komande za kompajliranje
```bash
# Clean previous build
make clean

# Build the kernel
make

# Run in QEMU emulator
make qemu

# Run with GDB debugging
make qemu-gdb
```

### Test Execution / Pokretanje testova
```bash
# Run specific test (1-12)
# Test options:
# 1-7:  Original system tests
# 8:    GetThreadId system call test
# 9:    Resource management test
# 10:   Ping system call test
# 11:   Priority semaphore test
# 12:   SetMaximumThreads test
```

## 🔧 System Calls / Sistemski pozivi

### Core System Calls
- `thread_create()` - Create new thread
- `thread_exit()` - Terminate current thread
- `thread_dispatch()` - Yield CPU to other threads
- `thread_join()` - Wait for thread completion
- `sem_open()`, `sem_close()`, `sem_wait()`, `sem_signal()` - Semaphore operations
- `mem_alloc()`, `mem_free()` - Memory management
- `time_sleep()` - Thread sleep functionality

### Extended System Calls
- `getThreadId()` - Get current thread ID
- `ping(thread_t handle)` - Ping thread for monitoring
- `SetMaximumThreads(int num)` - Set thread limit

## 📊 Memory Management / Upravljanje memorijom

### Memory Allocator Features
- **Free List Management**: Efficient block allocation and deallocation
- **Block Splitting**: Minimizes memory waste for small allocations
- **Block Coalescing**: Reduces fragmentation by merging adjacent free blocks
- **8-Byte Alignment**: Ensures proper memory access on RISC-V
- **Thread Tracking**: Monitors memory usage per thread for the ping system

### Memory Layout
```
Memory Address Space:
┌─────────────────────────────────────┐
│           Kernel Code               │
├─────────────────────────────────────┤
│           Kernel Data               │
├─────────────────────────────────────┤
│           Stack                     │
├─────────────────────────────────────┤
│           Heap                      │ ← HEAP_START_ADDR
│  ┌─────────────────────────────┐    │
│  │      Free Block List        │    │
│  │  ┌─────┐ ┌─────┐ ┌─────┐    │    │
│  │  │Block│ │Block│ │Block│    │    │
│  │  │Size │ │Size │ │Size │    │    │
│  │  │Next │ │Next │ │Next │    │    │
│  │  └─────┘ └─────┘ └─────┘    │    │
│  └─────────────────────────────┘    │
├─────────────────────────────────────┤
│           Unused Memory             │
└─────────────────────────────────────┘ ← HEAP_END_ADDR
```

## 🧵 Thread Management / Upravljanje nitima

### Thread Control Block (TCB)
- **Thread ID**: Unique identifier for each thread
- **Context**: Register state and stack information
- **Status**: Running, blocked, finished, sleeping states
- **Memory Tracking**: Allocation count for monitoring
- **Execution Time**: Start time and duration tracking

### Scheduling Features
- **Cooperative Multitasking**: Threads yield CPU voluntarily
- **Preemptive Multitasking**: Timer interrupts can preempt threads
- **Priority Support**: Priority-based semaphore scheduling
- **Thread Limits**: Configurable maximum active thread count

## 🔄 Interrupt Handling / Upravljanje prekidima

### RISC-V Interrupt System
The system uses the `stvecVectorTable` to handle all interrupts and exceptions:

```assembly
_ZN5Riscv16stvecVectorTableEv:
    j softwareInterruptHandler    # System calls (ecall)
    j timerInterruptHandler       # Timer interrupts
    .skip 28                      # Reserved entries
    j consoleHandler              # Console I/O interrupts
    sret                          # Return from supervisor mode
```

### Interrupt Types
- **Software Interrupts**: System calls from user programs
- **Timer Interrupts**: Periodic system maintenance and thread scheduling
- **Console Interrupts**: Keyboard input and display output

## 🧪 Testing / Testiranje

### Test Suite
The project includes comprehensive tests for all implemented features:

1. **Thread Management Tests**: Basic thread creation and scheduling
2. **Semaphore Tests**: Synchronization primitive functionality
3. **Memory Tests**: Allocation and deallocation patterns
4. **System Call Tests**: Extended functionality validation
5. **Resource Management Tests**: Thread-safe resource handling
6. **Priority Tests**: Priority-based scheduling validation

### Running Tests
```bash
# Build and run
make
make qemu

# In QEMU, select test number (1-12)
# Each test demonstrates different system capabilities
```

## 📚 Documentation / Dokumentacija

### Technical Documentation
- **stvecVectorTable_Explanation.md**: RISC-V interrupt system details
- **MemoryAllocator_Explanation.md**: Memory management system overview
- **Enhanced Code Comments**: Comprehensive inline documentation

### Key Concepts
- **Library Kernel**: Shared address space design
- **Embedded System**: Pre-loaded program configuration
- **RISC-V Architecture**: Modern processor architecture support
- **Virtual Environment**: Emulator-based development and testing

## 🔍 Debugging and Development / Debugovanje i razvoj

### Debugging Tools
- **GDB Integration**: Remote debugging with QEMU
- **Console Output**: Comprehensive logging and status reporting
- **Memory Monitoring**: Thread memory usage tracking
- **Performance Metrics**: Execution time and context switch monitoring

### Development Workflow
1. **Code Modification**: Edit source files in `src/` and `h/` directories
2. **Build**: Use `make` to compile changes
3. **Test**: Run in QEMU emulator
4. **Debug**: Use GDB for step-by-step execution
5. **Validate**: Run comprehensive test suite

## 🤝 Contributing / Doprinos projektu

### Development Guidelines
- **Code Style**: Follow existing C++ and assembly conventions
- **Documentation**: Add comments for complex algorithms
- **Testing**: Create tests for new features
- **Architecture**: Maintain clean separation between layers

### Adding New Features
1. **System Calls**: Add constants, declarations, and implementations
2. **Thread Features**: Extend TCB class and related functionality
3. **Memory Features**: Enhance allocator with new capabilities
4. **Tests**: Create comprehensive test scenarios

## 📄 License / Licenca

This project is developed for educational purposes as part of an operating systems course.

## 🙏 Acknowledgments / Zahvalnice

- **RISC-V Foundation**: For the open processor architecture
- **QEMU Project**: For the RISC-V emulation environment
- **Educational Community**: For operating system development resources

---

**Note**: This README provides an overview of the RISC-V operating system kernel project. For detailed implementation information, refer to the source code comments and technical documentation files.

**Napomena**: Ovaj README fajl pruža pregled projekta jezgra operativnog sistema RISC-V. Za detaljne informacije o implementaciji, pogledajte komentare u izvornom kodu i tehničku dokumentaciju. 
