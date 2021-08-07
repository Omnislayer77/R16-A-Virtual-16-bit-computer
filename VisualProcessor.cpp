#include "VisualProcessor.hpp"

VisualProcessor::VisualProcessor(uint16_t* datBus, uint16_t* addrBus)
{
    FILE* charSetFile;
    charSetFile = fopen("char set.bin", "rb");
    fread(charSet, 1, 0xff*8, charSetFile);
    fclose(charSetFile);

    addressBus = addrBus;
    dataBus = datBus;
    for(int i = 0; i < 2048; i++)
    {
        charMemory[i] = 0;
    }
    
    for(int i = 0; i < 128000; i++)
    {
        videoMemory[i] = 0;
    }
}

void VisualProcessor::updateState(bool RW)
{
    if(*addressBus >= 0xD800 && *addressBus <= 0xDFFF)
    {
        if(RW == true)  //Will output if the processor is reading and will write when the processor is writing
        {
            *dataBus = charMemory[*addressBus - 0xD800];
        }
        
        else
        {
            charMemory[*addressBus - 0xD800] = *dataBus;
        }
    }
}

void VisualProcessor::displayContents(sf::RenderWindow* window)
{
    std::vector<sf::Vertex> vertices;
    for(int x = 0; x < 50; x++)
    {
        for(int y = 0; y < 40; y++)
        {
            for(int i = 0; i < 8; i++)
            {
                for(int j = 0; j < 8; j++)
                {
                    tmpVideoMemory[(x * 8 + i) + (y * 8 + j) * 400] = videoMemory[(x * 8 + i) + (y * 8 + j) * 400];
                    if((charSet[8 * (charMemory[x + y * 50 + 0x30] & 0xff) + j] & (1 << (7-i))) >> (7-i) == 1)
                    {
                        videoMemory[(x * 8 + i) + (y * 8 + j) * 400] = (charMemory[x + y * 50 + 0x30]) >> 8;
                    }
                    
                    else
                    {
                        videoMemory[(x * 8 + i) + (y * 8 + j) * 400] = (charMemory[0x2f] & 0xff00) >> 8;
                    }
                }
            }
        }
    }
    
    for(int y = 0; y < 320; y++)  // display grid to screen
    {
        for(int x = 0; x < 400; x++)
        {
            if(videoMemory[x + y * 400] != tmpVideoMemory[x + y * 400]) // only update the pixel if it has changed since the last frame
            {
                vertices.push_back(sf::Vertex(sf::Vector2f(2 * x + 0, 2 * y + 0)));  // push the verticies of each corner of the pixel
                vertices.push_back(sf::Vertex(sf::Vector2f(2 * x + 2, 2 * y + 0)));
                vertices.push_back(sf::Vertex(sf::Vector2f(2 * x + 2, 2 * y + 2)));
                vertices.push_back(sf::Vertex(sf::Vector2f(2 * x + 0, 2 * y + 2)));
                
                sf::Color color = sf::Color(
                    videoMemory[x + y * 400] & 0b11100000,          // 3 bits for red
                    (videoMemory[x + y * 400] & 0b00011100) << 3,    // 3 bits for green
                    (videoMemory[x + y * 400] & 0b00000011) << 6);   // 2 bits for blue
                    
                vertices[vertices.size() - 1].color = color;  // set each corner of the pixel to the pixels color
                vertices[vertices.size() - 2].color = color;
                vertices[vertices.size() - 3].color = color;
                vertices[vertices.size() - 4].color = color;
            }
        }
    }
    
    (*window).draw(&vertices[0], vertices.size(), sf::Quads);  // display pixels to screen
    (*window).display();
}

VisualProcessor::~VisualProcessor()
{
    delete[] charMemory;
    delete[] videoMemory;
    delete[] tmpVideoMemory;
    delete[] charSet;
}
