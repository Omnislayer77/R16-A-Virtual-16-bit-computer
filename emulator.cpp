#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <vector>

#include "CPU.hpp"
#include "RAM.hpp"
#include "VisualProcessor.hpp"
#include "ROM.hpp"
#include "IO.hpp"

int main()
{
    uint16_t dataBus = 0;     // data bus for components to transfer data
    uint16_t addressBus = 0;  // address bus for components to address each other

    CPU cpu = CPU(&dataBus, &addressBus); 
    RAM ram = RAM(&dataBus, &addressBus);
    ROM rom = ROM(&dataBus, &addressBus);
    IO  io  = IO(&dataBus, &addressBus);
    VisualProcessor vp = VisualProcessor(&dataBus, &addressBus);


    sf::RenderWindow window(sf::VideoMode(800, 640), "R16 V2.0");
    
    sf::Clock Timer;
    sf::Clock frameCounter;
    int frames = 0;

    window.setKeyRepeatEnabled(false);
    while(window.isOpen())
    {
        frames++;

        cpu.updateState();                                       // update the state of all the components
        ram.updateState(cpu.RW);
        io.updateState(cpu.RW, &window);
        vp.updateState(cpu.RW);
        rom.updateState(cpu.RW);

        if(Timer.getElapsedTime().asMilliseconds() > 1000 / 20)  // update the screen 20 times a second
        {
            vp.displayContents(&window);
            Timer.restart();
        }

        if(frameCounter.getElapsedTime().asMilliseconds() > 5000) // print current clockrate to console
        {
            std::cout << frames/5 << " Hz" << std::endl;
            frames = 0;
            frameCounter.restart();
        }
    }
    
    return 0;
}
