#pragma once

#include "address_register.cpp"

class DataRegister
{
public:
    DataRegister(AddressRegister *address) : address(address) {}

    void write(unsigned char byte)
    {
        unsigned short addr = address->read();
    }

    unsigned char read()
    {
        // buffer
    }

private:
    AddressRegister *address;
};