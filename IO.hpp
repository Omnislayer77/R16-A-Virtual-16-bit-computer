#ifndef IO_HPP
#define IO_HPP

#include <SFML/Graphics.hpp>

class IO
{
public:
    uint16_t* dataBus;
    uint16_t* addressBus;

    uint8_t lastChar = 0;

    uint16_t* memory = new uint16_t[256];
    uint16_t* hardDisk = new uint16_t[16777216];

    IO(uint16_t* _dataBus, uint16_t* _addressBus);
    void updateState(bool RW, sf::RenderWindow* window);
    ~IO();
};

#endif /* IO_HPP */
