#include <algorithm>
#include <bitset>
#include <iostream>
#include <sstream>

#include "cpu.h"
#include "execution_data.h"
#include "../bus.h"

#define STOP_ON_BRK
// #define NES_LOG_TEST
#define INSTRUCTIONS_TEST


// Called when new cartridge inserted
void Cpu::resetInterrupt()
{
    pc = bus->read_16(bus->RESET_VECTOR_ADDR);
    resetState();
}

// Reset flags and registers
// https://wiki.nesdev.org/w/index.php/CPU_power_up_state
void Cpu::resetState()
{
    status = 0x34;
    #ifdef NES_LOG_TEST
        status = 0x24; // Log of nestest differs from in the irrelevant bits 5 and 4
    #endif
    #ifdef INSTRUCTIONS_TEST
        status = 0;
    #endif
    a = 0;
    x = 0;
    y = 0;
    sp = 0xfd;
}

void Cpu::run()
{
    int i = 0;
    resetInterrupt();

    while (true)
    {
        execData = new ExecutionData();
        unsigned char opCode = bus->read(pc);
        execData->pc = pc;
        
        #ifdef STOP_ON_BRK
            if (opCode == 0x00) {
                delete execData;
                break;
            }
        #endif
    
        pc++;
        execData->setRegisters(a, x, y, sp, status);
        execOpCode(opCode);

        #ifdef NES_LOG_TEST
            execData->logLine();
        #endif
        delete execData;

        #ifdef NES_LOG_TEST
            if (i++ > 6000)
                exit(0);
        #endif
    }

    print();
}

void Cpu::execOpCode(unsigned char opCode)
{
    execData->opCode = opCode;

    switch (opCode)
    {
    case 0x00:
        execData->opCodeName = "BRK";
        return brk();
    case 0x01:
        execData->opCodeName = "ORA";
        return ora(INDEXED_INDIRECT);
    case 0x05:
        execData->opCodeName = "ORA";
        return ora(ZERO_PAGE);
    case 0x06:
        execData->opCodeName = "ASL";
        asl(ZERO_PAGE);
        return;
    case 0x08:
        execData->opCodeName = "PHP";
        return php();
    case 0x09:
        execData->opCodeName = "ORA";
        return ora(IMMEDIATE);
    case 0x0a:
        execData->opCodeName = "ASL";
        asl(ACCUMULATOR);
        return;
    case 0x0d:
        execData->opCodeName = "ORA";
        return ora(ABSOLUTE);
    case 0x0e:
        execData->opCodeName = "ASL";
        asl(ABSOLUTE);
        return;
    case 0x10:
        execData->opCodeName = "BPL";
        return bpl();
    case 0x11:
        execData->opCodeName = "ORA";
        return ora(INDIRECT_INDEXED);
    case 0x15:
        execData->opCodeName = "ORA";
        return ora(ZERO_PAGE_X);
    case 0x16:
        execData->opCodeName = "ASL";
        asl(ZERO_PAGE_X);
        return;
    case 0x18:
        execData->opCodeName = "CLC";
        return clc();
    case 0x19:
        execData->opCodeName = "ORA";
        return ora(ABSOLUTE_Y);
    case 0x1d:
        execData->opCodeName = "ORA";
        return ora(ABSOLUTE_X);
    case 0x1e:
        execData->opCodeName = "ASL";
        asl(ABSOLUTE_X);
        return;
    case 0x20:
        execData->opCodeName = "JSR";
        return jsr();
    case 0x21:
        execData->opCodeName = "AND";
        return andOp(INDEXED_INDIRECT);
    case 0x24:
        execData->opCodeName = "BIT";
        return bit(ZERO_PAGE);
    case 0x25:
        execData->opCodeName = "AND";
        return andOp(ZERO_PAGE);
    case 0x26:
        execData->opCodeName = "ROL";
        rol(ZERO_PAGE);
        return;
    case 0x28:
        execData->opCodeName = "PLP";
        return plp();
    case 0x29:
        execData->opCodeName = "AND";
        return andOp(IMMEDIATE);
    case 0x2a:
        execData->opCodeName = "ROL";
        rol(ACCUMULATOR);
        return;
    case 0x2c:
        execData->opCodeName = "BIT";
        return bit(ABSOLUTE);
    case 0x2d:
        execData->opCodeName = "AND";
        return andOp(ABSOLUTE);
    case 0x2e:
        execData->opCodeName = "ROL";
        rol(ABSOLUTE);
        return;
    case 0x30:
        execData->opCodeName = "BMI";
        return bmi();
    case 0x31:
        execData->opCodeName = "AND";
        return andOp(INDIRECT_INDEXED);
    case 0x35:
        execData->opCodeName = "AND";
        return andOp(ZERO_PAGE_X);
    case 0x36:
        execData->opCodeName = "ROL";
        rol(ZERO_PAGE_X);
        return;
    case 0x38:
        execData->opCodeName = "SEC";
        return sec();
    case 0x39:
        execData->opCodeName = "AND";
        return andOp(ABSOLUTE_Y);
    case 0x3d:
        execData->opCodeName = "AND";
        return andOp(ABSOLUTE_X);
    case 0x3e:
        execData->opCodeName = "ROL";
        rol(ABSOLUTE_X);
        return;
    case 0x40:
        execData->opCodeName = "RTI";
        return rti();
    case 0x41:
        execData->opCodeName = "EOR";
        return eor(INDEXED_INDIRECT);
    case 0x45:
        execData->opCodeName = "EOR";
        return eor(ZERO_PAGE);
    case 0x46:
        execData->opCodeName = "LSR";
        lsr(ZERO_PAGE);
        return;
    case 0x48:
        execData->opCodeName = "PHA";
        return pha();
    case 0x49:
        execData->opCodeName = "EOR";
        return eor(IMMEDIATE);
    case 0x4a:
        execData->opCodeName = "LSR";
        lsr(ACCUMULATOR);
        return;
    case 0x4c:
        execData->opCodeName = "JMP";
        return jmp(ABSOLUTE);
    case 0x4d:
        execData->opCodeName = "EOR";
        return eor(ABSOLUTE);
    case 0x4e:
        execData->opCodeName = "LSR";
        lsr(ABSOLUTE);
        return;
    case 0x50:
        execData->opCodeName = "BVC";
        return bvc();
    case 0x51:
        execData->opCodeName = "EOR";
        return eor(INDIRECT_INDEXED);
    case 0x55:
        execData->opCodeName = "EOR";
        return eor(ZERO_PAGE_X);
    case 0x56:
        execData->opCodeName = "LSR";
        lsr(ZERO_PAGE_X);
        return;
    case 0x58:
        execData->opCodeName = "CLI";
        return cli();
    case 0x59:
        execData->opCodeName = "EOR";
        return eor(ABSOLUTE_Y);
    case 0x5d:
        execData->opCodeName = "EOR";
        return eor(ABSOLUTE_X);
    case 0x5e:
        execData->opCodeName = "LSR";
        lsr(ABSOLUTE_X);
        return;
    case 0x60:
        execData->opCodeName = "RTS";
        return rts();
    case 0x61:
        execData->opCodeName = "ADC";
        return adc(INDEXED_INDIRECT);
    case 0x65:
        execData->opCodeName = "ADC";
        return adc(ZERO_PAGE);
    case 0x66:
        execData->opCodeName = "ROR";
        ror(ZERO_PAGE);
        return;
    case 0x68:
        execData->opCodeName = "PLA";
        return pla();
    case 0x69:
        execData->opCodeName = "ADC";
        return adc(IMMEDIATE);
    case 0x6a:
        execData->opCodeName = "ROR";
        ror(ACCUMULATOR);
        return;
    case 0x6c:
        execData->opCodeName = "JMP";
        return jmp(INDIRECT);
    case 0x6d:
        execData->opCodeName = "ADC";
        return adc(ABSOLUTE);
    case 0x6e:
        execData->opCodeName = "ROR";
        ror(ABSOLUTE);
        return;
    case 0x70:
        execData->opCodeName = "BVS";
        return bvs();
    case 0x71:
        execData->opCodeName = "ADC";
        return adc(INDIRECT_INDEXED);
    case 0x75:
        execData->opCodeName = "ADC";
        return adc(ZERO_PAGE_X);
    case 0x76:
        execData->opCodeName = "ROR";
        ror(ZERO_PAGE_X);
        return;
    case 0x78:
        execData->opCodeName = "SEI";
        return sei();
    case 0x79:
        execData->opCodeName = "ADC";
        return adc(ABSOLUTE_Y);
    case 0x7d:
        execData->opCodeName = "ADC";
        return adc(ABSOLUTE_X);
    case 0x7e:
        execData->opCodeName = "ROR";
        ror(ABSOLUTE_X);
        return;
    case 0x81:
        execData->opCodeName = "STA";
        return sta(INDEXED_INDIRECT);
    case 0x84:
        execData->opCodeName = "STY";
        return sty(ZERO_PAGE);
    case 0x85:
        execData->opCodeName = "STA";
        return sta(ZERO_PAGE);
    case 0x86:
        execData->opCodeName = "STX";
        return stx(ZERO_PAGE);
    case 0x88:
        execData->opCodeName = "DEY";
        return dey();
    case 0x8a:
        execData->opCodeName = "TXA";
        return txa();
    case 0x8c:
        execData->opCodeName = "STY";
        return sty(ABSOLUTE);
    case 0x8d:
        execData->opCodeName = "STA";
        return sta(ABSOLUTE);
    case 0x8e:
        execData->opCodeName = "STX";
        return stx(ABSOLUTE);
    case 0x90:
        execData->opCodeName = "BCC";
        return bcc();
    case 0x91:
        execData->opCodeName = "STA";
        return sta(INDIRECT_INDEXED);
    case 0x94:
        execData->opCodeName = "STY";
        return sty(ZERO_PAGE_X);
    case 0x95:
        execData->opCodeName = "STA";
        return sta(ZERO_PAGE_X);
    case 0x96:
        execData->opCodeName = "STX";
        return stx(ZERO_PAGE_Y);
    case 0x98:
        execData->opCodeName = "TYA";
        return tya();
    case 0x99:
        execData->opCodeName = "STA";
        return sta(ABSOLUTE_Y);
    case 0x9a:
        execData->opCodeName = "TXS";
        return txs();
    case 0x9d:
        execData->opCodeName = "STA";
        return sta(ABSOLUTE_X);
    case 0xa0:
        execData->opCodeName = "LDY";
        return ldy(IMMEDIATE);
    case 0xa1:
        execData->opCodeName = "LDA";
        return lda(INDEXED_INDIRECT);
    case 0xa2:
        execData->opCodeName = "LDX";
        return ldx(IMMEDIATE);
    case 0xa4:
        execData->opCodeName = "LDY";
        return ldy(ZERO_PAGE);
    case 0xa5:
        execData->opCodeName = "LDA";
        return lda(ZERO_PAGE);
    case 0xa6:
        execData->opCodeName = "LDX";
        return ldx(ZERO_PAGE);
    case 0xa8:
        execData->opCodeName = "TAY";
        return tay();
    case 0xa9:
        execData->opCodeName = "LDA";
        return lda(IMMEDIATE);
    case 0xaa:
        execData->opCodeName = "TAX";
        return tax();
    case 0xac:
        execData->opCodeName = "LDY";
        return ldy(ABSOLUTE);
    case 0xad:
        execData->opCodeName = "LDA";
        return lda(ABSOLUTE);
    case 0xae:
        execData->opCodeName = "LDX";
        return ldx(ABSOLUTE);
    case 0xb0:
        execData->opCodeName = "BCS";
        return bcs();
    case 0xb1:
        execData->opCodeName = "LDA";
        return lda(INDIRECT_INDEXED);
    case 0xb4:
        execData->opCodeName = "LDY";
        return ldy(ZERO_PAGE_X);
    case 0xb5:
        execData->opCodeName = "LDA";
        return lda(ZERO_PAGE_X);
    case 0xb6:
        execData->opCodeName = "LDX";
        return ldx(ZERO_PAGE_Y);
    case 0xb8:
        execData->opCodeName = "CLV";
        return clv();
    case 0xb9:
        execData->opCodeName = "LDA";
        return lda(ABSOLUTE_Y);
    case 0xba:
        execData->opCodeName = "TSX";
        return tsx();
    case 0xbc:
        execData->opCodeName = "LDY";
        return ldy(ABSOLUTE_X);
    case 0xbd:
        execData->opCodeName = "LDA";
        return lda(ABSOLUTE_X);
    case 0xbe:
        execData->opCodeName = "LDX";
        return ldx(ABSOLUTE_Y);
    case 0xc0:
        execData->opCodeName = "CPY";
        return cpy(IMMEDIATE);
    case 0xc1:
        execData->opCodeName = "CMP";
        return cmp(INDEXED_INDIRECT);
    case 0xc4:
        execData->opCodeName = "CPY";
        return cpy(ZERO_PAGE);
    case 0xc5:
        execData->opCodeName = "CMP";
        return cmp(ZERO_PAGE);
    case 0xc6:
        execData->opCodeName = "DEC";
        dec(ZERO_PAGE);
        return;
    case 0xc8:
        execData->opCodeName = "INY";
        return iny();
    case 0xc9:
        execData->opCodeName = "CMP";
        return cmp(IMMEDIATE);
    case 0xca:
        execData->opCodeName = "DEX";
        return dex();
    case 0xcc:
        execData->opCodeName = "CPY";
        return cpy(ABSOLUTE);
    case 0xcd:
        execData->opCodeName = "CMP";
        return cmp(ABSOLUTE);
    case 0xce:
        execData->opCodeName = "DEC";
        dec(ABSOLUTE);
        return;
    case 0xd0:
        execData->opCodeName = "BNE";
        return bne();
    case 0xd1:
        execData->opCodeName = "CMP";
        return cmp(INDIRECT_INDEXED);
    case 0xd5:
        execData->opCodeName = "CMP";
        return cmp(ZERO_PAGE_X);
    case 0xd6:
        execData->opCodeName = "DEC";
        dec(ZERO_PAGE_X);
        return;
    case 0xd8:
        execData->opCodeName = "CLD";
        return cld();
    case 0xd9:
        execData->opCodeName = "CMP";
        return cmp(ABSOLUTE_Y);
    case 0xdd:
        execData->opCodeName = "CMP";
        return cmp(ABSOLUTE_X);
    case 0xde:
        execData->opCodeName = "DEC";
        dec(ABSOLUTE_X);
        return;
    case 0xe0:
        execData->opCodeName = "CPX";
        return cpx(IMMEDIATE);
    case 0xe1:
        execData->opCodeName = "SBC";
        return sbc(INDEXED_INDIRECT);
    case 0xe4:
        execData->opCodeName = "CPX";
        return cpx(ZERO_PAGE);
    case 0xe5:
        execData->opCodeName = "SBC";
        return sbc(ZERO_PAGE);
    case 0xe6:
        execData->opCodeName = "INC";
        inc(ZERO_PAGE);
        return;
    case 0xe8:
        execData->opCodeName = "INX";
        return inx();
    case 0xe9:
        execData->opCodeName = "SBC";
        return sbc(IMMEDIATE);
    case 0xea:
        execData->opCodeName = "NOP";
        return nop(IMPLIED);
    case 0xec:
        execData->opCodeName = "CPX";
        return cpx(ABSOLUTE);
    case 0xed:
        execData->opCodeName = "SBC";
        return sbc(ABSOLUTE);
    case 0xee:
        execData->opCodeName = "INC";
        inc(ABSOLUTE);
        return;
    case 0xf0:
        execData->opCodeName = "BEQ";
        return beq();
    case 0xf1:
        execData->opCodeName = "SBC";
        return sbc(INDIRECT_INDEXED);
    case 0xf5:
        execData->opCodeName = "SBC";
        return sbc(ZERO_PAGE_X);
    case 0xf6:
        execData->opCodeName = "INC";
        inc(ZERO_PAGE_X);
        return;
    case 0xf8:
        execData->opCodeName = "SED";
        return sed();
    case 0xf9:
        execData->opCodeName = "SBC";
        return sbc(ABSOLUTE_Y);
    case 0xfd:
        execData->opCodeName = "SBC";
        return sbc(ABSOLUTE_X);
    case 0xfe:
        execData->opCodeName = "INC";
        inc(ABSOLUTE_X);
        return;

    // Illegal opcodes
    case 0x1a: case 0x3a: case 0x5a: case 0x7a: case 0xda: case 0xfa: // NOP
    case 0x02: case 0x12: case 0x22: case 0x32: case 0x42: case 0x52: case 0x62: case 0x72: case 0x92: case 0xB2: case 0xD2: case 0xF2: // JAM
        execData->opCodeName = "*NOP";
        return nop(IMPLIED);
    case 0x80: case 0x82: case 0x89: case 0xc2: case 0xe2:
    case 0x8b: // ANE
        execData->opCodeName = "*NOP";
        return nop(IMMEDIATE);
    case 0x04: case 0x44: case 0x64:
        execData->opCodeName = "*NOP";
        return nop(ZERO_PAGE);
    case 0x14: case 0x34: case 0x54: case 0x74: case 0xd4: case 0xf4:
        execData->opCodeName = "*NOP";
        return nop(ZERO_PAGE_X);
    case 0x0c:
        execData->opCodeName = "*NOP";
        return nop(ABSOLUTE);
    case 0x1c: case 0x3c: case 0x5c: case 0x7c: case 0xdc: case 0xfc:
        execData->opCodeName = "*NOP";
        return nop(ABSOLUTE_X);
    case 0x07:
        execData->opCodeName = "*SLO";
        return slo(ZERO_PAGE);
    case 0x17:
        execData->opCodeName = "*SLO";
        return slo(ZERO_PAGE_X);
    case 0x0f:
        execData->opCodeName = "*SLO";
        return slo(ABSOLUTE);
    case 0x1f:
        execData->opCodeName = "*SLO";
        return slo(ABSOLUTE_X);
    case 0x1b:
        execData->opCodeName = "*SLO";
        return slo(ABSOLUTE_Y);
    case 0x03:
        execData->opCodeName = "*SLO";
        return slo(INDEXED_INDIRECT);
    case 0x13:
        execData->opCodeName = "*SLO";
        return slo(INDIRECT_INDEXED);
    case 0x27:
        execData->opCodeName = "*RLA";
        return rla(ZERO_PAGE);
    case 0x37:
        execData->opCodeName = "*RLA";
        return rla(ZERO_PAGE_X);
    case 0x2f:
        execData->opCodeName = "*RLA";
        return rla(ABSOLUTE);
    case 0x3f:
        execData->opCodeName = "*RLA";
        return rla(ABSOLUTE_X);
    case 0x3b:
        execData->opCodeName = "*RLA";
        return rla(ABSOLUTE_Y);
    case 0x23:
        execData->opCodeName = "*RLA";
        return rla(INDEXED_INDIRECT);
    case 0x33:
        execData->opCodeName = "*RLA";
        return rla(INDIRECT_INDEXED);
    case 0x47:
        execData->opCodeName = "*SRE";
        return sre(ZERO_PAGE);
    case 0x57:
        execData->opCodeName = "*SRE";
        return sre(ZERO_PAGE_X);
    case 0x4f:
        execData->opCodeName = "*SRE";
        return sre(ABSOLUTE);
    case 0x5f:
        execData->opCodeName = "*SRE";
        return sre(ABSOLUTE_X);
    case 0x5b:
        execData->opCodeName = "*SRE";
        return sre(ABSOLUTE_Y);
    case 0x43:
        execData->opCodeName = "*SRE";
        return sre(INDEXED_INDIRECT);
    case 0x53:
        execData->opCodeName = "*SRE";
        return sre(INDIRECT_INDEXED);
    case 0x67:
        execData->opCodeName = "*RRA";
        return rra(ZERO_PAGE);
    case 0x77:
        execData->opCodeName = "*RRA";
        return rra(ZERO_PAGE_X);
    case 0x6f:
        execData->opCodeName = "*RRA";
        return rra(ABSOLUTE);
    case 0x7f:
        execData->opCodeName = "*RRA";
        return rra(ABSOLUTE_X);
    case 0x7b:
        execData->opCodeName = "*RRA";
        return rra(ABSOLUTE_Y);
    case 0x63:
        execData->opCodeName = "*RRA";
        return rra(INDEXED_INDIRECT);
    case 0x73:
        execData->opCodeName = "*RRA";
        return rra(INDIRECT_INDEXED);
    case 0x87:
        execData->opCodeName = "*SAX";
        return sax(ZERO_PAGE);
    case 0x97:
        execData->opCodeName = "*SAX";
        return sax(ZERO_PAGE_Y);
    case 0x8f:
        execData->opCodeName = "*SAX";
        return sax(ABSOLUTE);
    case 0x83:
        execData->opCodeName = "*SAX";
        return sax(INDEXED_INDIRECT);
    case 0xa7:
        execData->opCodeName = "*LAX";
        return lax(ZERO_PAGE);
    case 0xb7:
        execData->opCodeName = "*LAX";
        return lax(ZERO_PAGE_Y);
    case 0xaf:
        execData->opCodeName = "*LAX";
        return lax(ABSOLUTE);
    case 0xbf:
        execData->opCodeName = "*LAX";
        return lax(ABSOLUTE_Y);
    case 0xa3:
        execData->opCodeName = "*LAX";
        return lax(INDEXED_INDIRECT);
    case 0xb3:
        execData->opCodeName = "*LAX";
        return lax(INDIRECT_INDEXED);
    case 0xc7:
        execData->opCodeName = "*DCP";
        return dcp(ZERO_PAGE);
    case 0xd7:
        execData->opCodeName = "*DCP";
        return dcp(ZERO_PAGE_X);
    case 0xcf:
        execData->opCodeName = "*DCP";
        return dcp(ABSOLUTE);
    case 0xdf:
        execData->opCodeName = "*DCP";
        return dcp(ABSOLUTE_X);
    case 0xdb:
        execData->opCodeName = "*DCP";
        return dcp(ABSOLUTE_Y);
    case 0xc3:
        execData->opCodeName = "*DCP";
        return dcp(INDEXED_INDIRECT);
    case 0xd3:
        execData->opCodeName = "*DCP";
        return dcp(INDIRECT_INDEXED);
    case 0x4b:
        execData->opCodeName = "*ALR";
        return alr(IMMEDIATE);
    case 0x0b: case 0x2b:
        execData->opCodeName = "*ANC";
        return anc(IMMEDIATE);
    case 0x6b:
        execData->opCodeName = "*ARR";
        return arr(IMMEDIATE);
    case 0xe7:
        execData->opCodeName = "*ISC";
        return isc(ZERO_PAGE);
    case 0xf7:
        execData->opCodeName = "*ISC";
        return isc(ZERO_PAGE_X);
    case 0xef:
        execData->opCodeName = "*ISC";
        return isc(ABSOLUTE);
    case 0xff:
        execData->opCodeName = "*ISC";
        return isc(ABSOLUTE_X);
    case 0xfb:
        execData->opCodeName = "*ISC";
        return isc(ABSOLUTE_Y);
    case 0xe3:
        execData->opCodeName = "*ISC";
        return isc(INDEXED_INDIRECT);
    case 0xf3:
        execData->opCodeName = "*ISC";
        return isc(INDIRECT_INDEXED);

    default:
        if (opCode == 0x9f || opCode == 0x93) {
            std::cout << "UNSTABLE OPCODE SHA: " << std::hex << (int)opCode << std::endl;
        } else {
            std::cout << "UNKNOWN OPCODE: " << std::hex << (int)opCode << std::endl;
        }
        exit(1);
    }
}

// The BRK instruction forces the generation of an interrupt request. The program counter and processor status are pushed on the stack then the IRQ interrupt vector at $FFFE/F is loaded into the PC and the break flag in the status set to one.
void Cpu::brk()
{
    pushStack_16(pc);
    pushStack(status);
    pc = bus->read_16(bus->BREAK_VECTOR_ADDR);
    status = status | 0b0001'0000;
}

// The NOP instruction causes no changes to the processor other than the normal incrementing of the program counter to the next instruction.
void Cpu::nop(AddressingMode addressingMode)
{
    getAddress(addressingMode); // Because of illegal opcodes
}

// Adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void Cpu::adc(AddressingMode addressingMode)
{
    adc_value(bus->read(getAddress(addressingMode)));
}

void Cpu::adc_value(unsigned char value)
{
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

// Subtracts the contents of a memory location to the accumulator together with the not of the carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
void Cpu::sbc(AddressingMode addressingMode)
{
    sbc_value(bus->read(getAddress(addressingMode)));
}

void Cpu::sbc_value(unsigned char value)
{
    unsigned short result = a - (value + (getCarry() ? 0 : 1));
    updateZeroAndNegativeFlag(result);
    
    // clear carry flag if bit 7 overflow
    if (result > 0xff)
        status = status & 0b1111'1110;
    else
        status = status | 0b0000'0001;

    // Overflow flag. Set if result if two's complement is outside -128, +127 range.
    // This can only happen if:
    // - Positive minus negative, and the result is a negative number.
    // - Negative minus positive, and the result is a positive number.
    // Simplification: Sign of first input and result are different. Sign of second input and result are same.
    // Overflow occurs if (M^result) & (~N^result) & 0b1000'0000 is nonzero.
    if ((a ^ result) & ((~value) ^ result) & 0b1000'0000)
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

// Stores the contents of the accumulator into memory.
void Cpu::sta(AddressingMode addressingMode)
{
    unsigned short address = getAddress(addressingMode);
    bus->write_8(address, a);
}

// Stores the contents of the X register into memory.
void Cpu::stx(AddressingMode addressingMode)
{
    unsigned short address = getAddress(addressingMode);
    bus->write_8(address, x);
}

// Stores the contents of the Y register into memory.
void Cpu::sty(AddressingMode addressingMode)
{
    unsigned short address = getAddress(addressingMode);
    bus->write_8(address, y);
}

// Clears the interrupt disable flag allowing normal interrupt requests to be serviced.
void Cpu::cli()
{
    status = status & 0b1111'1011;
}

// A logical AND is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
void Cpu::andOp(AddressingMode addressingMode)
{
    a = a & bus->read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(a);
}

// An inclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
void Cpu::ora(AddressingMode addressingMode)
{
    a = a | bus->read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(a);
}

// An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
void Cpu::eor(AddressingMode addressingMode)
{
    a = a ^ bus->read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(a);
}

// Test if one or more bits are set in a target memory location. The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the result is not kept. 
// Bits 7 and 6 of the value from memory are copied into the N and V flags.
void Cpu::bit(AddressingMode addressingMode)
{
    unsigned char mem = bus->read(getAddress(addressingMode));
    unsigned char result = a & mem;
    updateZeroFlag(result);
    updateNegativeFlag(mem);
    updateOverflowFlag(mem);
}

// This operation shifts all the bits of the accumulator or memory contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the carry flag.
unsigned char Cpu::asl(AddressingMode addressingMode)
{
    unsigned char originalValue;
    unsigned char modifiedValue;
    if (addressingMode == ACCUMULATOR) {
        originalValue = a;
        a = a << 1;
        modifiedValue = a;
        execData->address = "A";
    } else {
        unsigned short addr = getAddress(addressingMode);
        originalValue = bus->read(addr);
        modifiedValue = originalValue << 1;
        bus->write_8(addr, modifiedValue);
    }

    updateZeroAndNegativeFlag(modifiedValue);
    status = (status & ~0x01) | (originalValue >> 7); // set carry flag
    return modifiedValue;
}

// Each of the bits in A or M is shift one place to the right. The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
unsigned char Cpu::lsr(AddressingMode addressingMode)
{
    unsigned char originalValue;
    unsigned char modifiedValue;
    if (addressingMode == ACCUMULATOR) {
        originalValue = a;
        a = a >> 1;
        modifiedValue = a;
        execData->address = "A";
    } else {
        unsigned short addr = getAddress(addressingMode);
        originalValue = bus->read(addr);
        modifiedValue = originalValue >> 1;
        bus->write_8(addr, modifiedValue);
    }

    updateZeroAndNegativeFlag(modifiedValue);
    status = (status & ~0x01) | (originalValue & 0x01); // set carry flag
    return modifiedValue;
}

// Move each of the bits in either A or M one place to the left. Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag value.
unsigned char Cpu::rol(AddressingMode addressingMode)
{
    unsigned char originalValue;
    unsigned char modifiedValue;
    if (addressingMode == ACCUMULATOR) {
        originalValue = a;
        a = (a << 1) | (status & 0x01);
        modifiedValue = a;
        execData->address = "A";
    } else {
        unsigned short address = getAddress(addressingMode);
        originalValue = bus->read(address);
        modifiedValue = (originalValue << 1) | (status & 0x01);
        bus->write_8(address, modifiedValue);
    }

    updateZeroAndNegativeFlag(modifiedValue);
    unsigned char newCarry = (originalValue & 0b1000'0000) >> 7;
    status = (status & ~0x01) | newCarry; // set carry flag
    return modifiedValue;
}

// Move each of the bits in either A or M one place to the right. Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.
unsigned char Cpu::ror(AddressingMode addressingMode)
{
    unsigned char originalValue;
    unsigned char modifiedValue;
    if (addressingMode == ACCUMULATOR) {
        originalValue = a;
        a = (a >> 1) | ((status & 0x01) << 7);
        modifiedValue = a;
        execData->address = "A";
    } else {
        unsigned short address = getAddress(addressingMode);
        originalValue = bus->read(address);
        modifiedValue = (originalValue >> 1) | ((status & 0x01) << 7);
        bus->write_8(address, modifiedValue);
    }

    updateZeroAndNegativeFlag(modifiedValue);
    unsigned char newCarry = (originalValue & 0b0000'0001);
    status = (status & ~0x01) | newCarry; // set carry flag
    return modifiedValue;
}

// The JSR instruction pushes the address (minus one) of the return point on to the stack and then sets the program counter to the target memory address.
void Cpu::jsr()
{
    unsigned short address = getAddress(ABSOLUTE);
    pushStack_16(pc - 1);
    pc = address;
}

// The RTS instruction is used at the end of a subroutine to return to the calling routine. It pulls the program counter (minus one) from the stack.
void Cpu::rts()
{
    pc = pullStack_16() + 1;
}

// Set the carry flag to zero.
void Cpu::clc()
{
    status = status & 0b1111'1110;
}

// Set the decimal flag to zero.
void Cpu::cld()
{
    status = status & 0b1111'0111;
}

// Clears the overflow flag.
void Cpu::clv()
{
    status = status & 0b1011'1111;
}

// Sets the program counter to the address specified by the operand.
void Cpu::jmp(AddressingMode addressingMode)
{
    unsigned short address;

    // 6502 does not correctly fetch the target address if the indirect vector falls on a page boundary (e.g. $xxFF where xx is any value from $00 to $FF). 
    // In this case fetches the LSB from $xxFF as expected but takes the MSB from $xx00.
    if (addressingMode == INDIRECT && bus->read(pc) == 0xff) {
        unsigned short addr = bus->read_16(pc);
        unsigned short p1 = bus->read(addr);
        unsigned short p2 = bus->read(addr & 0xff00);
        address = (p2 << 8) | p1;
        execData->param1 = {bus->read(pc)};
        execData->param2 = {bus->read(pc+1)};
        execData->address = "($" + ExecutionData::toHex_16(addr) + ") = " + ExecutionData::toHex_16(address);
    } else {
        address = getAddress(addressingMode);
    }
    
    pc = address;
}

// If the carry flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bcc()
{
    execData->param1 = {bus->read_signed(pc)};
    execData->address = "$" + ExecutionData::toHex_16(pc + bus->read_signed(pc) + 1);
    if (getCarry() == 0)
        pc += bus->read_signed(pc);
    pc++;
}

// If the carry flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bcs()
{
    execData->param1 = {bus->read_signed(pc)};
    execData->address = "$" + ExecutionData::toHex_16(pc + bus->read_signed(pc) + 1);
    if (getCarry() == 1)
        pc += bus->read_signed(pc);
    pc++;
}

// If the zero flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::beq()
{
    execData->param1 = {bus->read_signed(pc)};
    execData->address = "$" + ExecutionData::toHex_16(pc + bus->read_signed(pc) + 1);
    if (getZero() == 1)
        pc += bus->read_signed(pc);
    pc++;
}

// If the zero flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bne()
{
    execData->param1 = {bus->read_signed(pc)};
    execData->address = "$" + ExecutionData::toHex_16(pc + bus->read_signed(pc) + 1);
    if (getZero() == 0)
        pc += bus->read_signed(pc);
    pc++;
}

// If the negative flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bmi()
{
    execData->param1 = {bus->read_signed(pc)};
    execData->address = "$" + ExecutionData::toHex_16(pc + bus->read_signed(pc) + 1);
    if (getNegative() == 1)
        pc += bus->read_signed(pc);
    pc++;
}

// If the negative flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bpl()
{
    execData->param1 = {bus->read_signed(pc)};
    execData->address = "$" + ExecutionData::toHex_16(pc + bus->read_signed(pc) + 1);
    if (getNegative() == 0)
        pc += bus->read_signed(pc);
    pc++;
}

// If the overflow flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bvc()
{
    execData->param1 = {bus->read_signed(pc)};
    execData->address = "$" + ExecutionData::toHex_16(pc + bus->read_signed(pc) + 1);
    if (getOverflow() == 0)
        pc += bus->read_signed(pc);
    pc++;
}

// If the overflow flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void Cpu::bvs()
{
    execData->param1 = {bus->read_signed(pc)};
    execData->address = "$" + ExecutionData::toHex_16(pc + bus->read_signed(pc) + 1);
    if (getOverflow() == 1)
        pc += bus->read_signed(pc);
    pc++;
}

// Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
void Cpu::lda(AddressingMode addressingMode)
{
    a = bus->read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(a);
}

// Loads a byte of memory into the X register setting the zero and negative flags as appropriate.
void Cpu::ldx(AddressingMode addressingMode)
{
    x = bus->read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(x);
}

// Loads a byte of memory into the Y register setting the zero and negative flags as appropriate.
void Cpu::ldy(AddressingMode addressingMode)
{
    y = bus->read(getAddress(addressingMode));
    updateZeroAndNegativeFlag(y);
}

// Copies the current contents of the accumulator into the X register and sets the zero and negative flags as appropriate.
void Cpu::tax()
{
    x = a;
    updateZeroAndNegativeFlag(x);
}

// Copies the current contents of the accumulator into the Y register and sets the zero and negative flags as appropriate.
void Cpu::tay()
{
    y = a;
    updateZeroAndNegativeFlag(y);
}

// Copies the current contents of the stack register into the X register and sets the zero and negative flags as appropriate.
void Cpu::tsx()
{
    x = sp;
    updateZeroAndNegativeFlag(x);
}

// Copies the current contents of the X register into the accumulator and sets the zero and negative flags as appropriate.
void Cpu::txa()
{
    a = x;
    updateZeroAndNegativeFlag(a);
}

// Copies the current contents of the X register into the stack register.
void Cpu::txs()
{
    sp = x;
}

// Copies the current contents of the Y register into the accumulator and sets the zero and negative flags as appropriate.
void Cpu::tya()
{
    a = y;
    updateZeroAndNegativeFlag(a);
}

// This instruction compares the contents of the accumulator with another memory held value and sets the zero and carry flags as appropriate.
void Cpu::cmp(AddressingMode addressingMode)
{
    unsigned char mem = bus->read(getAddress(addressingMode));
    cmp_value(mem);
}

void Cpu::cmp_value(unsigned char value)
{
    unsigned char result = a - value;

    // Set carry flag if A >= M
    if (a >= value) {
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
    unsigned char mem = bus->read(getAddress(addressingMode));
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
    unsigned char mem = bus->read(getAddress(addressingMode));
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

// Pushes a copy of the accumulator on to the stack.
void Cpu::pha()
{
    pushStack(a);
}

// Pushes a copy of the status flags on to the stack.
// B flag: https://wiki.nesdev.org/w/index.php?title=Status_flags
void Cpu::php()
{
    pushStack(status | 0b0011'0000);
}

// Pulls an 8 bit value from the stack and into the accumulator. The zero and negative flags are set as appropriate.
void Cpu::pla()
{
    a = pullStack();
    updateZeroAndNegativeFlag(a);
}

// Pulls an 8 bit value from the stack and into the processor flags. The flags will take on new states as determined by the value pulled.
// Ignore b flag: https://wiki.nesdev.org/w/index.php?title=Status_flags
void Cpu::plp()
{
    #ifdef NES_LOG_TEST
        status = (pullStack() & 0b1110'1111) | 0b0010'0000; // Not clearing 5 because of test log. Doesn't matter because register doesn't exist.
    #else
        status = pullStack() & 0b1100'1111; // bit 5 and 4 do not exist and should be ignored.
    #endif
}

// The RTI instruction is used at the end of an interrupt processing routine. It pulls the processor flags from the stack followed by the program counter.
// Ignore b flag: https://wiki.nesdev.org/w/index.php?title=Status_flags
void Cpu::rti()
{
    
    #ifdef NES_LOG_TEST
        status = pullStack() & 0b1110'1111 | 0b0010'0000; // Not clearing 5 because of test log. Doesn't matter because register doesn't exist.
    #else
        status = pullStack() & 0b1100'1111; // bit 5 and 4 do not exist and should be ignored.
    #endif
    pc = pullStack_16();
}

// Subtracts one from the value held at a specified memory location setting the zero and negative flags as appropriate.
unsigned char Cpu::dec(AddressingMode addressingMode)
{
    unsigned short addr = getAddress(addressingMode);
    unsigned char mem = bus->read(addr);
    bus->write_8(addr, mem - 1);
    updateZeroAndNegativeFlag(mem - 1);
    return mem - 1;
}

// Subtracts one from the X register setting the zero and negative flags as appropriate.
void Cpu::dex()
{
    x = x - 1;
    updateZeroAndNegativeFlag(x);
}

// Subtracts one from the Y register setting the zero and negative flags as appropriate.
void Cpu::dey()
{
    y = y - 1;
    updateZeroAndNegativeFlag(y);
}

// Adds one to the value held at a specified memory location setting the zero and negative flags as appropriate.
unsigned char Cpu::inc(AddressingMode addressingMode)
{
    unsigned short addr = getAddress(addressingMode);
    unsigned char mem = bus->read(addr);
    bus->write_8(addr, mem + 1);
    updateZeroAndNegativeFlag(mem + 1);
    return mem + 1;
}

// Adds one to the X register setting the zero and negative flags as appropriate.
void Cpu::inx()
{
    x += 1;
    updateZeroAndNegativeFlag(x);
}

// Adds one to the Y register setting the zero and negative flags as appropriate.
void Cpu::iny()
{
    y += 1;
    updateZeroAndNegativeFlag(y);
}

// Shift left one bit in memory, then OR accumulator with memory.
// ASL oper + ORA oper
void Cpu::slo(AddressingMode addressingMode)
{
    unsigned char val = asl(addressingMode);
    a = a | val;
    updateZeroAndNegativeFlag(a);
}

// Rotate one bit left in memory, then AND accumulator with memory.
// ROL oper + AND oper
void Cpu::rla(AddressingMode addressingMode)
{
    unsigned char val = rol(addressingMode);
    a = a & val;
    updateZeroAndNegativeFlag(a);
}

// Shift right one bit in memory, then EOR accumulator with memory.
// LSR oper + EOR oper
void Cpu::sre(AddressingMode addressingMode)
{
    unsigned char val = lsr(addressingMode);
    a = a ^ val;
    updateZeroAndNegativeFlag(a);
}

// Rotate one bit right in memory, then add memory to accumulator (with carry).
// ROR oper + ADC oper
void Cpu::rra(AddressingMode addressingMode)
{
    unsigned char val = ror(addressingMode);
    adc_value(val);
}

// A and X are put on the bus at the same time (resulting effectively in an AND operation) and stored in M
void Cpu::sax(AddressingMode addressingMode)
{
    bus->write_8(getAddress(addressingMode), a & x);
}

// LDA oper + LDX oper
// Load accumulator and X register with memory.
void Cpu::lax(AddressingMode addressingMode)
{
    unsigned char val = bus->read(getAddress(addressingMode));
    x = val;
    a = val;
    updateZeroAndNegativeFlag(val);
}

// DEC oper + CMP oper
void Cpu::dcp(AddressingMode addressingMode)
{
    unsigned char value = dec(addressingMode);
    cmp_value(value);
}

// AND oper + LSR
// AND byte with accumulator, then shift right one bit in accumu-lator.
void Cpu::alr(AddressingMode addressingMode)
{
    andOp(addressingMode);
    lsr(ACCUMULATOR);
}

// AND byte with accumulator. If result is negative then carry is set.
void Cpu::anc(AddressingMode addressingMode)
{
    andOp(addressingMode);
    if (a & (1 << 7))
        status = status | 0b0000'0001;
    else
        status = status & 0b1111'1110;
}

// AND byte with accumulator, then rotate one bit right in accu-mulator  (AND oper + ROR)
// and check bit 5 and 6:
// - If both bits are 1: set C, clear V.
// - If both bits are 0: clear C and V.
// - If only bit 5 is 1: set V, clear C.
// - If only bit 6 is 1: set C and V.
void Cpu::arr(AddressingMode addressingMode)
{
    andOp(addressingMode);
    ror(ACCUMULATOR);

    unsigned char bit5 = (a & 0b0010'0000) >> 5;
    unsigned char bit6 = (a & 0b0100'0000) >> 6;
    
    if (bit6 == 1)
        status = status | 0b0000'0001; // set carry
    else
        status = status & 0b1111'1110; // clear carry
    
    if ((bit5 ^ bit6) == 1)
        status = status | 0b0100'0000; // set overflow
    else
        status = status & 0b1011'1111; // clear overflow

}

// INC oper + SBC oper
// Increase memory by one, then subtract memory from accu-mulator (with borrow).
void Cpu::isc(AddressingMode addressingMode)
{
    unsigned char value = inc(addressingMode);
    sbc_value(value);
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

void Cpu::pushStack(unsigned char value)
{
    bus->write_8(getSP_16(), value);
    sp--;
}

void Cpu::pushStack_16(unsigned short value)
{
    unsigned char p1 = value & 0x00ff;
    unsigned char p2 = value >> 8;
    pushStack(p2);
    pushStack(p1);
}

unsigned char Cpu::pullStack()
{
    sp++;
    unsigned char value = bus->read(getSP_16());
    return value;
}

unsigned short Cpu::pullStack_16()
{
    unsigned short p1 = pullStack();
    unsigned short p2 = pullStack();
    return (p2 << 8) | p1;
}

unsigned short Cpu::getAddress(AddressingMode addressingMode)
{
    unsigned short out;
    int increment = 1;

    switch (addressingMode)
    {
    case IMPLIED:
        return 0;
    case IMMEDIATE:
        out = pc;
        execData->param1 = {bus->read(pc)};
        execData->address = "#$" + ExecutionData::toHex(*execData->param1);
        break;
    case ZERO_PAGE:
        out = bus->read(pc);
        execData->param1 = {bus->read(pc)};
        execData->address = "$" + ExecutionData::toHex(*execData->param1) + " = " + ExecutionData::toHex(bus->read(*execData->param1));
        break;
    case ZERO_PAGE_X:
        out = (bus->read(pc) + x) % 256;
        execData->param1 = {bus->read(pc)};
        execData->address = "$" + ExecutionData::toHex(bus->read(pc)) + ",X @ " + ExecutionData::toHex(out) + " = " + ExecutionData::toHex(bus->read(out));
        break;
    case ZERO_PAGE_Y:
        out = (bus->read(pc) + y) % 256;
        execData->param1 = {bus->read(pc)};
        execData->address = "$" + ExecutionData::toHex(bus->read(pc)) + ",Y @ " + ExecutionData::toHex(out) + " = " + ExecutionData::toHex(bus->read(out));
        break;
    case ABSOLUTE:
    {
        out = bus->read_16(pc);
        execData->param1 = {bus->read(pc)};
        execData->param2 = {bus->read(pc+1)};
        execData->address = "$" + ExecutionData::toHex_16(out) + " = " + ExecutionData::toHex(bus->read(out));
        increment = 2;
        break;
    }
    case ABSOLUTE_X:
        out = bus->read_16(pc) + x;
        execData->param1 = {bus->read(pc)};
        execData->param2 = {bus->read(pc+1)};
        execData->address = "$" + ExecutionData::toHex_16(bus->read_16(pc)) + ",X @ " + ExecutionData::toHex_16(out) + " = " + ExecutionData::toHex(bus->read(out));
        increment = 2;
        break;
    case ABSOLUTE_Y:
        out = bus->read_16(pc) + y;
        execData->param1 = {bus->read(pc)};
        execData->param2 = {bus->read(pc+1)};
        execData->address = "$" + ExecutionData::toHex_16(bus->read_16(pc)) + ",Y @ " + ExecutionData::toHex_16(out) + " = " + ExecutionData::toHex(bus->read(out));
        increment = 2;
        break;
    case INDIRECT: 
    {
        unsigned short addr = bus->read_16(pc);
        out = bus->read_16(addr);
        execData->param1 = {bus->read(pc)};
        execData->param2 = {bus->read(pc+1)};
        execData->address = "($" + ExecutionData::toHex_16(addr) + ") = " + ExecutionData::toHex_16(out);
        increment = 2;
        break;
    }
    case INDEXED_INDIRECT:
    {
        unsigned char addr = (bus->read(pc) + x);
        out = bus->read_16_zero_page_wrap(addr);
        execData->address = "($" + ExecutionData::toHex(bus->read(pc)) + ",X) @ " + ExecutionData::toHex(addr) + " = " + ExecutionData::toHex_16(out) + " = " + ExecutionData::toHex(bus->read(out));
        execData->param1 = {bus->read(pc)};
        break;
    }
    case INDIRECT_INDEXED:
    {
        unsigned char addr = bus->read(pc);
        out = bus->read_16_zero_page_wrap(addr) + y;
        execData->address = "($" + ExecutionData::toHex(addr) + "),Y = " + ExecutionData::toHex_16(bus->read_16_zero_page_wrap(addr)) + " @ " + ExecutionData::toHex_16(out) + " = " + ExecutionData::toHex(bus->read(out));
        execData->param1 = {bus->read(pc)};
        break;
    }
    default:
        std::cout << "Unexpected addressing mode " << addressingMode << std::endl;
        exit(1);
    }

    pc += increment;
    return out;
}

void Cpu::print()
{
    std::cout << "Program Counter: " << pc << std::endl;
    std::cout << "Register A: " << std::hex << (int)a << std::endl;
    std::cout << "Register X: " << std::hex << (int)x << std::endl;
    std::cout << "Register Y: " << std::hex << (int)y << std::endl;
    std::cout << "Status: " << std::bitset<8>(status) << std::endl;
}