#include "ROM.hpp"

ROM::ROM(uint16_t* _dataBus, uint16_t* _addressBus)
{
    FILE* romFile; // read contents of rom.bin into rom memory
    romFile = fopen("rom.bin", "rb");
    fread(memory, 1, 16384, romFile);
    fclose(romFile);
    
    for(int i = 0; i < 8192; i++) // flip endianess from 0xBBAA to 0xAABB (little to big)
    {
        uint16_t tmp = memory[i] & 0x00FF;
        memory[i] = (tmp << 8) | (memory[i] >> 8);
    }
    
    addressBus = _addressBus;
    dataBus = _dataBus;
}

void ROM::updateState(bool RW)
{
    if(*addressBus >= 0xE000 && *addressBus <= 0xFFFF) // only read from ROM if the address is in the correct range
    {
        if(!RW) // check if the CPU is writing and block it (ROM is read-only)
        {
            std::cout << "Warning, Attempt to write to Read-Only Memory at address: " << std::hex << *addressBus << std::endl;
        }
        
        else // check if the CPU is writing and block it (ROM is read-only)
        {
            *dataBus = memory[*addressBus - 0xE000];
        }
    }
}

ROM::~ROM()
{
    delete[] memory;
}
