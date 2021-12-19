#include <algorithm>
#include <iostream>

#include "bus.h"

void Bus::insertDisk(Rom *rom)
{
    readData(rom->getPrgData(), rom->size);
}

void Bus::readData(unsigned char *data, int length)
{
    unsigned short start = 0x8000;
    write(start, data, length);
    write(0xc000, data, length); // TODO depends on ROM
}

void Bus::write(unsigned short address, unsigned char data[], int length)
{
    std::copy(data, data+length, memory + address);
}

void Bus::write_8(unsigned short address, unsigned char byte)
{
    memory[address] = byte;
}

// 16-bit values are stored in little-endian
void Bus::write_16(unsigned short address, unsigned short data)
{
    memory[address] = data & 0x00ff;
    memory[address + 1] = (data & 0xff00) >> 8;
}

unsigned char Bus::read(unsigned short address)
{
    return memory[address];
}

// 16-bit values are stored in little-endian
unsigned short Bus::read_16(unsigned short address)
{
    unsigned short p1 = memory[address]; 
    unsigned short p2 = memory[address+1];
    return (p2 << 8) | p1;
}

signed int Bus::read_signed(unsigned short address)
{
    unsigned char value = read(address);
    bool negative = (value >> 7) == 1;
    return negative ? -((unsigned char) (~value + 1)) : value;
}