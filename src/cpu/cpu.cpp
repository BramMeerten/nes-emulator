#include <algorithm>
#include <bitset>
#include <iostream>

#include "cpu.h"
#include "../system.h"

Cpu::Cpu(System *system)
{
    resetState();
    this->system = system;
}

// Called when new cartridge inserted
void Cpu::resetInterrupt()
{
    pc = system->memory.read_16(system->RESET_VECTOR_ADDR);
    resetState();
}

// Reset flags and registers
void Cpu::resetState()
{
    status = 0;
    a = 0;
    x = 0;
    y = 0;
}

void Cpu::run()
{
    while(true)
    {
        unsigned char opCode = system->memory.read(pc);
        if (opCode == 0x00) break;
        execOpCode(opCode);
        pc++;
    }

    print();
}

void Cpu::execOpCode(unsigned char opCode)
{
    switch (opCode)
    {
    case 0x21:
        return andOp(INDEXED_INDIRECT);
    case 0x25:
        return andOp(ZERO_PAGE);
    case 0x29:
        return andOp(IMMEDIATE);
    case 0x2d:
        return andOp(ABSOLUTE);
    case 0x31:
        return andOp(INDIRECT_INDEXED);
    case 0x35:
        return andOp(ZERO_PAGE_X);
    case 0x39:
        return andOp(ABSOLUTE_Y);
    case 0x3d:
        return andOp(ABSOLUTE_X);
    case 0xa0:
        return ldy(IMMEDIATE);
    case 0xa2:
        return ldx(IMMEDIATE);
    case 0xa4:
        return ldy(ZERO_PAGE);
    case 0xa5:
        return lda(ZERO_PAGE);
    case 0xa6:
        return ldx(ZERO_PAGE);
    case 0xa9:
        return lda(IMMEDIATE);
    case 0xac:
        return ldy(ABSOLUTE);
    case 0xad:
        return lda(ABSOLUTE);
    case 0xae:
        return ldx(ABSOLUTE);
    case 0xb1:
        return lda(INDIRECT_INDEXED);
    case 0xb4:
        return ldy(ZERO_PAGE_X);
    case 0xb5:
        return lda(ZERO_PAGE_X);
    case 0xb6:
        return ldx(ZERO_PAGE_Y);
    case 0xb9:
        return lda(ABSOLUTE_Y);
    case 0xbc:
        return ldy(ABSOLUTE_X);
    case 0xbd:
        return lda(ABSOLUTE_X);
    case 0xbe:
        return ldx(ABSOLUTE_Y);
    case 0xa1:
        return lda(INDEXED_INDIRECT);
    case 0xaa:
        return tax();
    case 0xe8:
        return inx();
    default:
        std::cout << "UNKNOWN OPCODE: " << std::hex << (int)opCode << std::endl;
        exit(1);
    }
}

// A logical AND is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
void Cpu::andOp(AddressingMode addressingMode)
{
    pc++;
    a = a & system->memory.read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(a);
}

// Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
void Cpu::lda(AddressingMode addressingMode)
{
    pc++;
    a = system->memory.read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(a);
}

// Loads a byte of memory into the X register setting the zero and negative flags as appropriate.
void Cpu::ldx(AddressingMode addressingMode)
{
    pc++;
    x = system->memory.read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(x);
}

// Loads a byte of memory into the Y register setting the zero and negative flags as appropriate.
void Cpu::ldy(AddressingMode addressingMode)
{
    pc++;
    y = system->memory.read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(y);
}

// Copies the current contents of the accumulator into the X register and sets the zero and negative flags as appropriate.
void Cpu::tax()
{
    x = a;
    updateZeroAndNegativeFlag(x);
}

// Adds one to the X register setting the zero and negative flags as appropriate.
void Cpu::inx()
{
    x += 1;
    updateZeroAndNegativeFlag(x);
}

void Cpu::updateZeroAndNegativeFlag(unsigned char result)
{
    // Set zero flag if result = 0
    if (result == 0)
        status = status | 0b00000010;
    else
        status = status & 0b11111101;

    // Set negative flag if bit 7 of result is set
    if (result & (1 << 7))
        status = status | 0b01000000;
    else
        status = status & 0b10111111;
}

unsigned short Cpu::getAddress(AddressingMode addressingMode)
{
    switch (addressingMode)
    {
    case IMMEDIATE:
        return pc;
    case ZERO_PAGE:
        return system->memory.read(pc);
    case ZERO_PAGE_X:
        return (getAddress(ZERO_PAGE) + x) % 256;
    case ZERO_PAGE_Y:
        return (getAddress(ZERO_PAGE) + y) % 256;
    case ABSOLUTE: {
        unsigned short value = system->memory.read_16(pc);
        this->pc = this->pc + 1;
        return value;
    }
    case ABSOLUTE_X:
        return getAddress(ABSOLUTE) + x;
    case ABSOLUTE_Y:
        return getAddress(ABSOLUTE) + y;
    // TODO
    // case INDIRECT: {
    //     unsigned short value = system->memory.read_16(pc);
    //     this->pc = this->pc + 1;
    //     return system->memory.read_16(value);
    // }
    case INDEXED_INDIRECT: {
        unsigned char value = (system->memory.read(pc) + x) % 256;
        return system->memory.read_16(value);
    }
    case INDIRECT_INDEXED: {
        unsigned char value = system->memory.read(pc);
        return system->memory.read_16(value) + y;
    }
    }
}

void Cpu::print()
{
    std::cout << "Program Counter: " << pc << std::endl;
    std::cout << "Register A: " << std::hex << (int)a << std::endl;
    std::cout << "Register X: " << std::hex << (int)x << std::endl;
    std::cout << "Status: " << std::bitset<8>(status) << std::endl;
}