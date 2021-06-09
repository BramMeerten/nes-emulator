#include <iostream>
#include <string>
#include <stdlib.h>

#include "cpu/cpu.h"
#include "memory.h"
#include "system.h"

using std::string;

int main(int, char**) {
    System system;

    unsigned char data[5] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};
    system.insertDisk(data, 5);

    // loadRom("/Users/brammeerten/src/nes/test/roms/01-implied.nes");
}