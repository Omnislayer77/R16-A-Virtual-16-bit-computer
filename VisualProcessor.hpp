#ifndef VISUALPROCESSOR_HPP
#define VISUALPROCESSOR_HPP

#include <cstdint>
#include <vector>
#include <SFML/Graphics.hpp>

class VisualProcessor
{
public:
    uint16_t* dataBus;
    uint16_t* addressBus;
    uint16_t* charMemory = new uint16_t[2048];
    uint8_t*  videoMemory = new uint8_t[128000];
    uint8_t*  tmpVideoMemory = new uint8_t[128000];
    uint8_t* charSet = new uint8_t[0xff*8];

    VisualProcessor(uint16_t* datBus, uint16_t* addrBus);
    void updateState(bool RW);
    void displayContents(sf::RenderWindow* window);
    ~VisualProcessor();
};

#endif /* VISUALPROCESSOR_HPP */
