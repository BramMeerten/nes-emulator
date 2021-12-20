#pragma once

#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>

#include "rom.cpp"

class Bus
{
public:

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

    void dump(unsigned short from, unsigned short to)
    {
        for (unsigned short addr = from; addr <= to; addr++)
        {
            // if (read(addr) == 0x4c) {
            std::cout << std::setfill(' ') << std::left << std::setw(6) << toHex_16(addr);
            std::cout << std::setfill(' ') << std::left << std:: setw(4) << toHex(read(addr));
            std::cout << std::endl;
            // }
        }
    }

private:
    unsigned char memory[0xffff];

    std::string toHex_16(unsigned short bytes)
    {
        std::ostringstream formatted;
        formatted << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << bytes;
        return formatted.str();
    }

    std::string toHex(unsigned char byte)
    {
        std::ostringstream formatted;
        formatted << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int) byte;
        return formatted.str();
    }
};