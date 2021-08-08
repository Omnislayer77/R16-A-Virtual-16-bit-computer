#ifndef ROM_HPP
#define ROM_HPP

#include <cstdint>
#include <iostream>

class ROM
{
public:
    uint16_t* dataBus;
    uint16_t* addressBus;
    uint16_t* memory = new uint16_t[8192];

    ROM(uint16_t* _dataBus, uint16_t* _addressBus);
    void updateState(bool RW);
    ~ROM();
};

#endif /* ROM_HPP */
