class Cpu
{
public:
    Cpu();
    void load(unsigned char data[], int size);
    void run();

    int pc;
    unsigned char memory[4096];
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

private:
    void execOpCode(unsigned char opCode);
    void updateZeroAndNegativeFlag(unsigned char result);
    void print();

    void lda();
    void tax();
    void inx();

    // int pc;
    // unsigned char memory[4096];
    // unsigned char a;
    // unsigned char x;

    // // NVss DIZC
    // // N: negative
    // // s
    // // s
    // // V: Overflow
    // // D: Decimal
    // // I: Interrupt Disable
    // // Z: Zero
    // // C: Carry
    // unsigned char status;
};