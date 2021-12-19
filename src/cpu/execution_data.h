#pragma once

#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>

class ExecutionData {
public:
    unsigned short pc;
    unsigned char opCode; 
    unsigned char a, x, y; 
    unsigned char sp, status; 
    std::optional<unsigned char> param1;
    std::optional<unsigned char> param2;
    std::string opCodeName;

    ExecutionData(): param1(std::nullopt), param2(std::nullopt) {
    }

    void setRegisters(unsigned char a, unsigned char x, unsigned char y, unsigned char sp, unsigned char status)
    {
        this->a = a;
        this->x = x;
        this->y = y;
        this->sp = sp;
        this->status = status;
    }

    void logLine()
    {
        // PC
        std::cout << std::setfill(' ') << std::left << std::setw(6) << toHex_16(pc);
        
        // CPU Opcode
        std::ostringstream opCodeFormatted;
        opCodeFormatted << toHex(opCode);
        if (param1) opCodeFormatted << ' ' << toHex(*param1);
        if (param2) opCodeFormatted << ' ' << toHex(*param2);
        std::cout << std::setfill(' ') << std::left << std::setw(10) << opCodeFormatted.str();

        // Assembly
        std::cout << std::setfill(' ') << std::left << std::setw(32) << opCodeName;

        // Registers
        std::cout << "A:" << std::hex << toHex(a);
        std::cout << " X:" << std::hex << toHex(x);
        std::cout << " Y:" << std::hex << toHex(y);
        std::cout << " P:" << std::hex << toHex(status);
        std::cout << " SP:" << std::hex << toHex(sp);

        std::cout << std::endl;
    }

private:
    std::string toHex(unsigned char byte)
    {
        std::ostringstream formatted;
        formatted << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int) byte;
        return formatted.str();
    }

    std::string toHex_16(unsigned short bytes)
    {
        std::ostringstream formatted;
        formatted << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << bytes;
        return formatted.str();
    }
};
