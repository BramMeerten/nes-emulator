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
    while (true)
    {
        unsigned char opCode = system->memory.read(pc);
        if (opCode == 0x00)
            break;
        execOpCode(opCode);
        pc++;
    }

    print();
}

void Cpu::execOpCode(unsigned char opCode)
{
    // TODO BRK not implemented
    switch (opCode)
    {
    case 0x01:
        return ora(INDEXED_INDIRECT);
    case 0x05:
        return ora(ZERO_PAGE);
    case 0x06:
        return asl(ZERO_PAGE);
    case 0x09:
        return ora(IMMEDIATE);
    case 0x0a:
        return asl(ACCUMULATOR);
    case 0x0d:
        return ora(ABSOLUTE);
    case 0x0e:
        return asl(ABSOLUTE);
    case 0x10:
        return bpl();
    case 0x11:
        return ora(INDIRECT_INDEXED);
    case 0x15:
        return ora(ZERO_PAGE_X);
    case 0x16:
        return asl(ZERO_PAGE_X);
    case 0x18:
        return clc();
    case 0x19:
        return ora(ABSOLUTE_Y);
    case 0x1d:
        return ora(ABSOLUTE_X);
    case 0x1e:
        return asl(ABSOLUTE_X);
    case 0x21:
        return andOp(INDEXED_INDIRECT);
    case 0x24:
        return bit(ZERO_PAGE);
    case 0x25:
        return andOp(ZERO_PAGE);
    case 0x29:
        return andOp(IMMEDIATE);
    case 0x2c:
        return bit(ABSOLUTE);
    case 0x2d:
        return andOp(ABSOLUTE);
    case 0x30:
        return bmi();
    case 0x31:
        return andOp(INDIRECT_INDEXED);
    case 0x35:
        return andOp(ZERO_PAGE_X);
    case 0x38:
        return sec();
    case 0x39:
        return andOp(ABSOLUTE_Y);
    case 0x3d:
        return andOp(ABSOLUTE_X);
    case 0x46:
        return lsr(ZERO_PAGE);
    case 0x4a:
        return lsr(ACCUMULATOR);
    case 0x4e:
        return lsr(ABSOLUTE);
    case 0x50:
        return bvc();
    case 0x56:
        return lsr(ZERO_PAGE_X);
    case 0x58:
        return cli();
    case 0x5e:
        return lsr(ABSOLUTE_X);
    case 0x61:
        return adc(INDEXED_INDIRECT);
    case 0x65:
        return adc(ZERO_PAGE);
    case 0x69:
        return adc(IMMEDIATE);
    case 0x6d:
        return adc(ABSOLUTE);
    case 0x70:
        return bvs();
    case 0x71:
        return adc(INDIRECT_INDEXED);
    case 0x75:
        return adc(ZERO_PAGE_X);
    case 0x78:
        return sei();
    case 0x79:
        return adc(ABSOLUTE_Y);
    case 0x7d:
        return adc(ABSOLUTE_X);
    case 0x90:
        return bcc();
    case 0xa0:
        return ldy(IMMEDIATE);
    case 0xa1:
        return lda(INDEXED_INDIRECT);
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
    case 0xaa:
        return tax();
    case 0xac:
        return ldy(ABSOLUTE);
    case 0xad:
        return lda(ABSOLUTE);
    case 0xae:
        return ldx(ABSOLUTE);
    case 0xb0:
        return bcs();
    case 0xb1:
        return lda(INDIRECT_INDEXED);
    case 0xb4:
        return ldy(ZERO_PAGE_X);
    case 0xb5:
        return lda(ZERO_PAGE_X);
    case 0xb6:
        return ldx(ZERO_PAGE_Y);
    case 0xb8:
        return clv();
    case 0xb9:
        return lda(ABSOLUTE_Y);
    case 0xbc:
        return ldy(ABSOLUTE_X);
    case 0xbd:
        return lda(ABSOLUTE_X);
    case 0xbe:
        return ldx(ABSOLUTE_Y);
    case 0xc0:
        return cpy(IMMEDIATE);
    case 0xc1:
        return cmp(INDEXED_INDIRECT);
    case 0xc4:
        return cpy(ZERO_PAGE);
    case 0xc5:
        return cmp(ZERO_PAGE);
    case 0xc9:
        return cmp(IMMEDIATE);
    case 0xcc:
        return cpy(ABSOLUTE);
    case 0xcd:
        return cmp(ABSOLUTE);
    case 0xd0:
        return bne();
    case 0xd1:
        return cmp(INDIRECT_INDEXED);
    case 0xd5:
        return cmp(ZERO_PAGE_X);
    case 0xd8:
        return cld();
    case 0xd9:
        return cmp(ABSOLUTE_Y);
    case 0xdd:
        return cmp(ABSOLUTE_X);
    case 0xea:
        return nop();
    case 0xe0:
        return cpx(IMMEDIATE);
    case 0xe4:
        return cpx(ZERO_PAGE);
    case 0xe8:
        return inx();
    case 0xec:
        return cpx(ABSOLUTE);
    case 0xf0:
        return beq();
    case 0xf8:
        return sed();
    default:
        std::cout << "UNKNOWN OPCODE: " << std::hex << (int)opCode << std::endl;
        exit(1);
    }
}

// The NOP instruction causes no changes to the processor other than the normal incrementing of the program counter to the next instruction.
void Cpu::nop()
{
}

// Adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void Cpu::adc(AddressingMode addressingMode)
{
    pc++;
    unsigned char value = system->memory.read(getAddress(addressingMode));
    unsigned short result = a + value + getCarry();
    updateZeroAndNegativeFlag(result);
    updateCarryFlag(result);

    // Overflow flag. Set if result if two's complement is outside -128, +127 range.
    // This can only happen if:
    // - Two positive numbers are added, and the result is a negative number.
    // - Two negative numbers are added, and the result is a positive number.
    // Simplification: Sign of both inputs is different from the sign of the result.
    // Overflow occurs if (M^result) & (N^result) & 0b1000'0000 is nonzero.
    if ((a ^ result) & (value ^ result) & 0b1000'0000)
        status = status | 0b0100'0000;
    else
        status = status & 0b1011'1111;

    a = result;
}

// Set the carry flag to one.
void Cpu::sec()
{
    status = status | 0b0000'0001;
}

// Set the decimal flag to one.
void Cpu::sed()
{
    status = status | 0b0000'1000;
}

// Set the interrupt disable flag to one.
void Cpu::sei()
{
    status = status | 0b0000'0100;
}

// Clears the interrupt disable flag allowing normal interrupt requests to be serviced.
void Cpu::cli()
{
    status = status & 0b1111'1011;
}

// A logical AND is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
void Cpu::andOp(AddressingMode addressingMode)
{
    pc++;
    a = a & system->memory.read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(a);
}

// An inclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
void Cpu::ora(AddressingMode addressingMode)
{
    pc++;
    a = a | system->memory.read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(a);
}

// Test if one or more bits are set in a target memory location. The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the result is not kept. 
// Bits 7 and 6 of the value from memory are copied into the N and V flags.
void Cpu::bit(AddressingMode addressingMode)
{
    pc++;
    unsigned char mem = system->memory.read(getAddress(addressingMode));
    unsigned char result = a & mem;
    updateZeroFlag(result);
    updateNegativeFlag(mem);
    updateOverflowFlag(mem);
}

// This operation shifts all the bits of the accumulator or memory contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the carry flag.
void Cpu::asl(AddressingMode addressingMode)
{
    unsigned char originalValue;
    if (addressingMode == ACCUMULATOR) {
        originalValue = a;
        a = a << 1;
        updateZeroAndNegativeFlag(a);
    } else {
        pc++;
        originalValue = system->memory.read(getAddress(addressingMode));
        system->memory.write_8(getAddress(addressingMode), originalValue << 1);
        updateZeroAndNegativeFlag(originalValue << 1);
    }

    status = (status & ~0x01) | (originalValue >> 7); // set carry flag
}

// Each of the bits in A or M is shift one place to the right. The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
void Cpu::lsr(AddressingMode addressingMode)
{
    unsigned char originalValue;
    if (addressingMode == ACCUMULATOR) {
        originalValue = a;
        a = a >> 1;
        updateZeroAndNegativeFlag(a);
    } else {
        pc++;
        originalValue = system->memory.read(getAddress(addressingMode));
        system->memory.write_8(getAddress(addressingMode), originalValue >> 1);
        updateZeroAndNegativeFlag(originalValue >> 1);
    }

    status = (status & ~0x01) | (originalValue & 0x01); // set carry flag
}

// Set the carry flag to zero.
void Cpu::clc()
{
    status = status & 0x1111'1110;
}

// Set the decimal flag to zero.
void Cpu::cld()
{
    status = status & 0x1111'0111;
}

// Clears the overflow flag.
void Cpu::clv()
{
    status = status & 0x1011'1111;
}

// If the carry flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bcc()
{
    pc++;
    if (getCarry() == 0)
        pc += system->memory.read_signed(pc);
}

// If the carry flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bcs()
{
    pc++;
    if (getCarry() == 1)
        pc += system->memory.read_signed(pc);
}

// If the zero flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::beq()
{
    pc++;
    if (getZero() == 1)
        pc += system->memory.read_signed(pc);
}

// If the zero flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bne()
{
    pc++;
    if (getZero() == 0)
        pc += system->memory.read_signed(pc);
}

// If the negative flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bmi()
{
    pc++;
    if (getNegative() == 1)
        pc += system->memory.read_signed(pc);
}

// If the negative flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bpl()
{
    pc++;
    if (getNegative() == 0)
        pc += system->memory.read_signed(pc);
}

// If the overflow flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bvc()
{
    pc++;
    if (getOverflow() == 0)
        pc += system->memory.read_signed(pc);
}

// If the overflow flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bvs()
{
    pc++;
    if (getOverflow() == 1)
        pc += system->memory.read_signed(pc);
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

// This instruction compares the contents of the accumulator with another memory held value and sets the zero and carry flags as appropriate.
void Cpu::cmp(AddressingMode addressingMode)
{
    pc++;
    unsigned char mem = system->memory.read(getAddress(addressingMode));
    unsigned char result = a - mem;

    // Set carry flag if A >= M
    if (a >= mem) {
        status = status | 0b0000'0001;
    } else {
        status = status & 0b1111'1110;
    }

    updateZeroFlag(result);
    updateNegativeFlag(result);
}

// This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
void Cpu::cpx(AddressingMode addressingMode)
{
    pc++;
    unsigned char mem = system->memory.read(getAddress(addressingMode));
    unsigned char result = x - mem;

    // Set carry flag if X >= M
    if (x >= mem) {
        status = status | 0b0000'0001;
    } else {
        status = status & 0b1111'1110;
    }

    updateZeroFlag(result);
    updateNegativeFlag(result);
}

// This instruction compares the contents of the Y register with another memory held value and sets the zero and carry flags as appropriate.
void Cpu::cpy(AddressingMode addressingMode)
{
    pc++;
    unsigned char mem = system->memory.read(getAddress(addressingMode));
    unsigned char result = y - mem;

    // Set carry flag if Y >= M
    if (y >= mem) {
        status = status | 0b0000'0001;
    } else {
        status = status & 0b1111'1110;
    }

    updateZeroFlag(result);
    updateNegativeFlag(result);
}

void Cpu::updateZeroAndNegativeFlag(unsigned char result)
{
    updateZeroFlag(result);
    updateNegativeFlag(result);
}

// Set zero flag if result = 0
void Cpu::updateZeroFlag(unsigned char result)
{
    if (result == 0)
        status = status | 0b00000010;
    else
        status = status & 0b11111101;
}

// Set negative flag if bit 7 of result is set
void Cpu::updateNegativeFlag(unsigned char result)
{
    if (result & (1 << 7))
        status = status | 0b1000'0000;
    else
        status = status & 0b0111'1111;
}

// TODO for now sets flag if bit 6 of result is set (used in BIT operator)
// But is this always the expected behaviour?
void Cpu::updateOverflowFlag(unsigned char result)
{
    if (result & (1 << 6))
        status = status | 0b0100'0000;
    else
        status = status & 0b1011'1111;
}

void Cpu::updateCarryFlag(unsigned short result)
{
    // set carry flag if bit 7 overflow
    if (result > 0xff)
        status = status | 0b0000'0001;
    else
        status = status & 0b1111'1110;
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
    case ABSOLUTE:
    {
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
    case INDEXED_INDIRECT:
    {
        unsigned char value = (system->memory.read(pc) + x) % 256;
        return system->memory.read_16(value);
    }
    case INDIRECT_INDEXED:
    {
        unsigned char value = system->memory.read(pc);
        return system->memory.read_16(value) + y;
    }
    default:
        std::cout << "Unexpected addressing mode " << addressingMode << std::endl;
        exit(1);
    }
}

void Cpu::print()
{
    std::cout << "Program Counter: " << pc << std::endl;
    std::cout << "Register A: " << std::hex << (int)a << std::endl;
    std::cout << "Register X: " << std::hex << (int)x << std::endl;
    std::cout << "Status: " << std::bitset<8>(status) << std::endl;
}