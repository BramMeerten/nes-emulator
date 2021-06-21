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

private:
    void resetState();
    void execOpCode(unsigned char opCode);
    void updateZeroAndNegativeFlag(unsigned char result);
    void updateCarryFlag(unsigned short result);
    void print();

    void adc(AddressingMode addressingMode);
    void sec();
    void andOp(AddressingMode addressingMode);
    void asl(AddressingMode AddressingMode);
    void clc();
    void bcc();
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
};
