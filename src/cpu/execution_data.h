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
    std::string address;

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
        std::cout << std::setfill(' ') << std::left << std::setw(9) << opCodeFormatted.str();

        // Assembly
        std::cout << (opCodeName.length() == 3 ? " " : "") << std::setfill(' ') << std::left << std::setw(4) << opCodeName << (opCodeName.length() == 3 ? "" : " ");

        // Address
        if (opCode == 0x4c || opCode == 0x20) { // Absolute JMP and JSR
            std::cout << std::setfill(' ') << std::left << std::setw(28) << ("$" + toHex(*param2) + toHex(*param1));
        } else {
            std::cout << std::setfill(' ') << std::left << std::setw(28) << address;
        }

        // Registers
        std::cout << "A:" << std::hex << toHex(a);
        std::cout << " X:" << std::hex << toHex(x);
        std::cout << " Y:" << std::hex << toHex(y);
        std::cout << " P:" << std::hex << toHex(status);
        std::cout << " SP:" << std::hex << toHex(sp);

        std::cout << std::endl;
    }

    static std::string toHex(unsigned char byte)
    {
        std::ostringstream formatted;
        formatted << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int) byte;
        return formatted.str();
    }

    static std::string toHex_16(unsigned short bytes)
    {
        std::ostringstream formatted;
        formatted << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << bytes;
        return formatted.str();
    }
};
