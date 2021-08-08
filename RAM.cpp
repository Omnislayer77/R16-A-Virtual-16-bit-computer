#include "RAM.hpp"

RAM::RAM(uint16_t* datBus, uint16_t* addrBus)
{
    for(int i = 0; i < 0xD700; i++)
    {
        memory[i] = 0;
    }

    addressBus = addrBus;
    dataBus = datBus;
}

void RAM::updateState(bool RW)
{
    if(*addressBus >= 0 && *addressBus <= 0xD6FF)
    {
        if(RW == true)
        {
            *dataBus = memory[*addressBus];
        }

        else
        {
            memory[*addressBus] = *dataBus;
        }
    }
}

RAM::~RAM()
{
    delete[] memory;
}
