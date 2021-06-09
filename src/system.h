#pragma once

#include "cpu/cpu.h"
#include "memory.h"

class System
{
public:
    System() : cpu(this) {}

    void insertDisk(unsigned char data[], int length)
    {
        unsigned short start = 0x8000;
        memory.write(start, data, length);
        memory.write_16(RESET_VECTOR_ADDR, start);
        cpu.resetInterrupt();
        cpu.run();
    }

    Memory memory;
    Cpu cpu;

    const unsigned short RESET_VECTOR_ADDR = 0xFFFC;
};
