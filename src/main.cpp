#include <iostream>
#include <string>
#include <stdlib.h>

#include "cpu/cpu.h"

using std::string;

int main(int, char**) {
    std::cout << "Hello, world!\n";
    Cpu cpu;
    unsigned char data[5] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};
    cpu.load(data, 5);
    cpu.run();
    // loadRom("/Users/brammeerten/src/nes/test/roms/01-implied.nes");
}
