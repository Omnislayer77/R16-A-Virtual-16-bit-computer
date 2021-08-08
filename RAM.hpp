#ifndef RAM_HPP
#define RAM_HPP

#include <cstdint>

class RAM
{
public:
    uint16_t* memory = new uint16_t[0xD700];
    uint16_t* dataBus;
    uint16_t* addressBus;

    RAM(uint16_t* datBus, uint16_t* addrBus);
    void updateState(bool RW);
    ~RAM();
};

#endif /* RAM_HPP */
