#pragma once

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
    unsigned char getStatus() { return status; };

private:
    void resetState();
    void execOpCode(unsigned char opCode);
    void updateZeroAndNegativeFlag(unsigned char result);
    void print();

    void lda();
    void tax();
    void inx();

    System *system;

    unsigned short pc;
    unsigned char a;
    unsigned char x;

    // NVss DIZC
    // N: negative
    // s
    // s
    // V: Overflow
    // D: Decimal
    // I: Interrupt Disable
    // Z: Zero
    // C: Carry
    unsigned char status;
};
