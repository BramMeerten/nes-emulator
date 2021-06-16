#include <algorithm>
#include <iostream>

#include "memory.h"

void Memory::write(unsigned short address, unsigned char data[], int length)
{
    std::copy(data, data+length, memory + address);
}

void Memory::write_8(unsigned short address, unsigned char byte)
{
    memory[address] = byte;
}

// 16-bit values are stored in little-endian
void Memory::write_16(unsigned short address, unsigned short data)
{
    memory[address] = data & 0x00ff;
    memory[address + 1] = (data & 0xff00) >> 8;
}

unsigned char Memory::read(unsigned short address)
{
    return memory[address];
}

// 16-bit values are stored in little-endian
unsigned short Memory::read_16(unsigned short address)
{
    unsigned short p1 = memory[address]; 
    unsigned short p2 = memory[address+1];
    return (p2 << 8) | p1;
}