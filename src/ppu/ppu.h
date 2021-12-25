#pragma once

#include "address_register.cpp"

class Ppu
{
public:

    Ppu() : data(DataRegister(address))
    {
        address = new AddressRegister();
    }

    ~Ppu()
    {
        delete address;
    }

    void setControl(unsigned char control) { this->control = control; }
    void setMask(unsigned char mask) { this->mask = mask; }
    void setOamAddress(unsigned char address) { this->oamAddress = address; }
    void setOamData(unsigned char data) { this->oamData = data; }
    void setScroll(unsigned char scroll) { this->scroll = scroll; }
    void setAddress(unsigned char byte) { this->address->write(byte); }
    void setData(unsigned char data) { this->data = data; }
    void setOamDma(unsigned char dma) { this->oamDma = dma; }

private:
    unsigned char control;
    unsigned char mask;
    unsigned char status;
    unsigned char oamAddress;
    unsigned char oamData;
    unsigned char scroll;
    AddressRegister *address;
    unsigned char data;
    unsigned char oamDma;
};