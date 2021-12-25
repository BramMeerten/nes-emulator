#pragma once

#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>

#include "ppu/ppu.h"
#include "rom.cpp"

class Bus
{
public:

    Bus(Ppu *ppu) : ppu(ppu) {}

    const unsigned short RESET_VECTOR_ADDR = 0xfffc;
    const unsigned short BREAK_VECTOR_ADDR = 0xfffe;

    void insertDisk(Rom *rom);
    void readData(unsigned char * data, int length);


    void write(unsigned short address, unsigned char data[], int length);
    void write_8(unsigned short address, unsigned char byte);
    void write_16(unsigned short address, unsigned short data);
    
    unsigned char read(unsigned short address);
    unsigned short read_16(unsigned short address);
    unsigned short read_16_zero_page_wrap(unsigned short address);
    signed int read_signed(unsigned short address);

private:
    Ppu *ppu;
    unsigned char memory[0xffff];
};