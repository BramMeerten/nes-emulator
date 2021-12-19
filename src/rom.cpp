#pragma once

#include <iostream>
#include <string>

using std::string;

class Header {
public:
    const int num16kbBanks;
    const bool hasTrainer;

    Header(int num16kbBanks, bool hasTrainer) : num16kbBanks(num16kbBanks), hasTrainer(hasTrainer) {}
};

class Rom
{
public:
    Rom(string file) : file(file), prg(NULL) {}
    int size = 0;

    unsigned char *getPrgData() {
        if (prg == NULL) {
            read();
        }
        return prg;
    }

private:

    const int HEADER_SIZE = 16;
    const int TRAINER_SIZE = 512;
    const int KB_16_IN_BYTES = 16 * 1024;

    string file;
    unsigned char *prg;

    void read()
    {
        FILE *file = fopen("../../test/roms/01.nes", "r+");
        if (file == NULL)
            throw std::invalid_argument("ROM file not found: " + this->file); // TODO custom exception

        Header header = readHeader(file);
        prg = readPrg(file, header);
        
        fclose(file);
    }

    Header readHeader(FILE *file) {
        unsigned char *header = (unsigned char *) malloc(HEADER_SIZE); // TODO not cleaned up
        fread(header, sizeof(unsigned char), HEADER_SIZE, file);
        
        string nesString = string(header, header+4);
        if ("NES" != nesString)
            throw std::invalid_argument("ROM has invalid start of header: " + nesString);

        unsigned char controlByte1 = header[6];
        bool hasTrainer = controlByte1 & 0b0000'0100;
        
        return Header(header[4], hasTrainer);
    }

    unsigned char *readPrg(FILE *file, Header &header)
    {
        int start = HEADER_SIZE + (header.hasTrainer ? TRAINER_SIZE : 0);
        int size = header.num16kbBanks * KB_16_IN_BYTES;

        unsigned char *data = (unsigned char *) malloc(size);
        fseek(file, start, SEEK_SET);
        fread(data, sizeof(unsigned char), size, file);
        this->size = size;
        return data;
    }
};
