#include <iostream>
#include <string>
#include <stdlib.h>

#include "cpu/cpu.h"
#include "bus.h"
#include "bus.h"
#include "rom.cpp"

using std::string;

int main(int, char**) {
    Bus bus;
    Rom rom("../../test/roms/01.nes");
    bus.insertDisk(&rom);
    bus.write_16(bus.RESET_VECTOR_ADDR, 0xc000);

    Cpu cpu(&bus);
    cpu.run();
}