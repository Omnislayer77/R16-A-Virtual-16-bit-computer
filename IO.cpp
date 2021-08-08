#include "IO.hpp"

IO::IO(uint16_t* _dataBus, uint16_t* _addressBus)
{
    FILE* hardDiskFile; // read contents of file into harddisk
    hardDiskFile = fopen("hardDisk.bin", "rb");
    fread(hardDisk, 2, 16777216, hardDiskFile);
    fclose(hardDiskFile);

    for(int i = 0; i < 256; i++) // null out memory
    {
        memory[i] = 0;
    }
    
    addressBus = _addressBus;
    dataBus = _dataBus;
}

void IO::updateState(bool RW, sf::RenderWindow* window)
{
    if(*addressBus >= 0xD700 && *addressBus <= 0xD7FF) // Make sure address is in the correct range
    {
        if(RW)  // Write to bus when CPU is reading and read from bus when CPU is writing
        {
            *dataBus = memory[*addressBus - 0xD700];
        }
        
        else
        {
            memory[*addressBus - 0xD700] = *dataBus;
        }
    }
    
    if(!memory[0x04])
    {
        hardDisk[(memory[0x01]<<16) + memory[0x02]] = memory[0x03];
    }
    
    else
    {
        memory[0x03] = hardDisk[(memory[0x01]<<16) + memory[0x02]];
    }
    
    sf::Event event;
    
    // while there are pending events...
    while ((*window).pollEvent(event)) // do keypresses and update correct memory location
    {
        // check the type of the event...
        switch (event.type)
        {
            case sf::Event::TextEntered:
            {
                if (event.text.unicode < 128)
                {
                    lastChar = event.text.unicode;
                    memory[0] = static_cast<unsigned char>(event.text.unicode); // set $D700 to the ASCII value of the current key pressed
                }
                
                break;
            }
            
            case sf::Event::KeyReleased:  // null out $D700 when no key is pressed
            {
                memory[0] = 0x0000;
                break;
            }
            
            case sf::Event::Closed: // make sure SFML closes when X in corner is pressed
            {
                FILE* hardDiskFile; // copy hardDisk to a file for longterm storage when the program closes
                hardDiskFile = fopen("hardDisk.bin", "wb");
                fwrite(hardDisk, 2, 16777216, hardDiskFile);
                fclose(hardDiskFile);
                (*window).close();
                break;
            }
            
            default:
            {
                break;
            }
        }
    }
}

IO::~IO()
{
    delete[] memory;
    delete[] hardDisk;
}
