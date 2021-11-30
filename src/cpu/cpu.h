#pragma once

#include "addressing_mode.cpp"

class System;

class Cpu
{
public:
    Cpu(System *system);
    void resetInterrupt();
    void run();

    int getPC() { return pc; };
    unsigned char getA() { return a; };
    unsigned char getX() { return x; };
    unsigned char getY() { return y; };
    unsigned char getSP() { return sp; };
    unsigned char getStatus() { return status; };
    unsigned char getZero() { return (status & 0b0000'0010) >> 1; }

private:
    void resetState();
    void execOpCode(unsigned char opCode);
    void updateZeroAndNegativeFlag(unsigned char result);
    void updateZeroFlag(unsigned char result);
    void updateNegativeFlag(unsigned char result);
    void updateOverflowFlag(unsigned char result);
    void updateCarryFlag(unsigned short result);
    void pushStack(unsigned char value);
    void pushStack_16(unsigned short value);
    unsigned char pullStack();
    unsigned short pullStack_16();
    void print();

    void nop();
    void adc(AddressingMode addressingMode);
    void sbc(AddressingMode addressingMode);
    void sec();
    void sed();
    void sei();
    void sta(AddressingMode addressingMode);
    void stx(AddressingMode addressingMode);
    void sty(AddressingMode addressingMode);
    void bit(AddressingMode addressingMode);
    void andOp(AddressingMode addressingMode);
    void ora(AddressingMode addressingMode);
    void eor(AddressingMode addressingMode);
    void asl(AddressingMode AddressingMode);
    void rol(AddressingMode AddressingMode);
    void ror(AddressingMode AddressingMode);
    void clc();
    void cld();
    void cli();
    void clv();
    void bcc();
    void bcs();
    void beq();
    void bne();
    void bmi();
    void bpl();
    void bvc();
    void bvs();
    void lda(AddressingMode addressingMode);
    void ldx(AddressingMode addressingMode);
    void ldy(AddressingMode addressingMode);
    void tax();
    void tay();
    void tsx();
    void txa();
    void txs();
    void tya();
    void inx();
    void cmp(AddressingMode addressingMode);
    void cpx(AddressingMode addressingMode);
    void cpy(AddressingMode addressingMode);
    void lsr(AddressingMode addressingMode);
    void jsr();
    void rts();
    void pha();
    void php();
    void pla();
    void plp();
    void rti();
    void dec(AddressingMode addressingMode);
    void dex();

    unsigned short getAddress(AddressingMode addressingMode);

    System *system;

    unsigned short pc;
    unsigned char sp;
    unsigned char a;
    unsigned char x;
    unsigned char y;

    // NVss DIZC
    // ----------
    // N: negative
    // V: Overflow
    // s
    // s
    // D: Decimal
    // I: Interrupt Disable
    // Z: Zero
    // C: Carry
    unsigned char status;

    unsigned short getSP_16()
    {
        return 0x0100 | (0x0100 | sp);
    }

    unsigned char getCarry()
    {
        return status & 0b0000'0001;
    }

    unsigned char getNegative()
    {
        return (status & 0b1000'0000) ? 1 : 0;
    }

    unsigned char getOverflow()
    {
        return (status & 0b0100'0000) ? 1 : 0;
    }
};
