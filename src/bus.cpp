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
    if (address == 0x2000)
        ppu->setControl(byte);
    else if (address == 0x2001)
        ppu->setMask(byte);
    else if (address == 0x2003)
        ppu->setOamAddress(byte);
    else if (address == 0x2004)
        ppu->setOamData(byte);
    else if (address == 0x2005)
        ppu->setScroll(byte);
    else if (address == 0x2006)
        ppu->setAddress(byte);
    else if (address == 0x2007)
        ppu->setData(byte);
    else if (address == 0x4014)
        ppu->setOamData(byte);
    else 
        memory[address] = byte; // TODO check if is RAM address
}

// 16-bit values are stored in little-endian
void Bus::write_16(unsigned short address, unsigned short data)
{
    write_8(address, data & 0x00ff);
    write_8(address + 1, (data & 0xff00) >> 8);
}

unsigned char Bus::read(unsigned short address)
{
    if (address == 0x2000 || address == 0x2001 || address == 0x2003 || address == 0x2005 || address == 0x2006 || address == 0x4014) {
        std::cout << "Read from PPU register " << std::hex << (int) address << "not allowed.";
        exit(1); // TODO exception
    }
    return memory[address]; // TODO mirrors
}

// 16-bit values are stored in little-endian
unsigned short Bus::read_16(unsigned short address)
{
    unsigned short p1 = read(address); 
    unsigned short p2 = read(address+1);
    return (p2 << 8) | p1;
}

unsigned short Bus::read_16_zero_page_wrap(unsigned short address)
{
    unsigned short p1 = read(address % 256);
    unsigned short p2 = read((address+1) % 256);
    return (p2 << 8) | p1;
}

signed int Bus::read_signed(unsigned short address)
{
    unsigned char value = read(address);
    bool negative = (value >> 7) == 1;
    return negative ? -((unsigned char) (~value + 1)) : value;
}