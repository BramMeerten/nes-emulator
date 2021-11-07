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
    void print();

    void adc(AddressingMode addressingMode);
    void sec();
    void sed();
    void sei();
    void bit(AddressingMode addressingMode);
    void andOp(AddressingMode addressingMode);
    void asl(AddressingMode AddressingMode);
    void clc();
    void cld();
    void cli();
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
    void inx();

    unsigned short getAddress(AddressingMode addressingMode);

    System *system;

    unsigned short pc;
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
