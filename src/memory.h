#pragma once

class Memory
{
public:
    void write(unsigned short address, unsigned char data[], int length);
    void write_8(unsigned short address, unsigned char byte);
    void write_16(unsigned short address, unsigned short data);
    
    unsigned char read(unsigned short address);
    unsigned short read_16(unsigned short address);

private:
    unsigned char memory[0xffff];
};