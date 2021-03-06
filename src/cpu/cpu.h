#pragma once

#include "addressing_mode.cpp"
#include "execution_data.h"

class Bus;

class Cpu
{
public:
    Cpu(Bus *bus) : execData(NULL), bus(bus) { }

    void run();

    int getPC() { return pc; };
    unsigned char getA() { return a; };
    unsigned char getX() { return x; };
    unsigned char getY() { return y; };
    unsigned char getSP() { return sp; };
    unsigned char getStatus() { return status; };
    unsigned char getZero() { return (status & 0b0000'0010) >> 1; }

private:
    void resetInterrupt();
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
    void logLine();

    void brk();
    void nop(AddressingMode addressingMode);
    void adc(AddressingMode addressingMode);
    void adc_value(unsigned char value);
    void sbc(AddressingMode addressingMode);
    void sbc_value(unsigned char value);
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
    unsigned char asl(AddressingMode addressingMode);
    unsigned char rol(AddressingMode addressingMode);
    unsigned char ror(AddressingMode addressingMode);
    void clc();
    void cld();
    void cli();
    void clv();
    void jmp(AddressingMode addressingMode);
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
    void cmp(AddressingMode addressingMode);
    void cmp_value(unsigned char value);
    void cpx(AddressingMode addressingMode);
    void cpy(AddressingMode addressingMode);
    unsigned char lsr(AddressingMode addressingMode);
    void jsr();
    void rts();
    void pha();
    void php();
    void pla();
    void plp();
    void rti();
    unsigned char dec(AddressingMode addressingMode);
    void dex();
    void dey();
    unsigned char inc(AddressingMode addressingMode);
    void inx();
    void iny();

    void slo(AddressingMode AddressingMode);
    void rla(AddressingMode AddressingMode);
    void sre(AddressingMode AddressingMode);
    void rra(AddressingMode AddressingMode);
    void sax(AddressingMode AddressingMode);
    void lax(AddressingMode AddressingMode);
    void dcp(AddressingMode AddressingMode);
    void alr(AddressingMode AddressingMode);
    void anc(AddressingMode AddressingMode);
    void arr(AddressingMode AddressingMode);
    void isb(AddressingMode AddressingMode);
    void las(AddressingMode AddressingMode);

    unsigned short getAddress(AddressingMode addressingMode);

    Bus *bus;
    ExecutionData *execData;

    unsigned short pc;
    unsigned char sp;
    unsigned char a;
    unsigned char x;
    unsigned char y;

    // NVsB DIZC
    // ----------
    // N: negative
    // V: Overflow
    // s
    // B: Break
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
