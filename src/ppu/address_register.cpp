#pragma once

class AddressRegister
{
public:
    AddressRegister() : is_high(true) {}

    void write(unsigned char byte)
    {
        if (is_high) 
            high = byte;
        else 
            low = byte;
        
        is_high = !is_high;
    }

    unsigned short read()
    {
        return (((unsigned short) high) << 8) | low;
    }

private:
    unsigned char low;
    unsigned char high;
    bool is_high;
};