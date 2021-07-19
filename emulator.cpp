#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <vector>

class CPU
{
public:
    uint16_t registers[16];         // 16 general purpose registers
    uint16_t instructionPointer;    // pointer that points to current instruction
    uint16_t stackPointer;          // 1 Kb of stack between $1400(top) and $1000
    uint16_t instructionRegister;   // Will hold the first word of the current instruction
    uint16_t microcodePointer;      // curerent stage of the current instruction
    uint8_t  flags;                 // Status flags in bit order of Negative, Zero, Carry, Halt ; 0b0000NZCH
    bool     RW;                    // true if the CPU is reading and false if the CPU is writing
    

    uint16_t* dataBus;
    uint16_t* addressBus;

    CPU(uint16_t* datBus, uint16_t* addrBus)
    {      
        addressBus = addrBus;
        dataBus = datBus;

        for(int i = 0; i < sizeof(registers)/sizeof(uint16_t); i++)
        {
            registers[i] = 0;
        }    
        instructionPointer = 0xE000;  
        stackPointer = 0x1400;   
        flags = 0x00;   
        RW = true; 
        microcodePointer =  0x0000;
    }


    void updateState()
    {
        microcodePointer++;
        //std::cout << microcodePointer << '\n' << std::endl;
        if(microcodePointer == 1)
        {
            *addressBus = instructionPointer;
            RW = true;
            instructionPointer++;
        } else if(microcodePointer == 2)
        {
            instructionRegister = *dataBus;
            //std::cout << instructionRegister << std::endl;
            *addressBus = instructionPointer;
            instructionPointer++;
        } else
        {
            //std::cout << instructionRegister << std::endl;
            switch((instructionRegister & 0xff00)>>8)
            {
            case 0x00:   // NOP
                microcodePointer = 0;
                break;

            case 0x01:   // HLT
                microcodePointer = 0;
                flags |= 0x01;
                break;

            case 0x10:   // MOV ADDRESS, REGISTER
                microcodePointer = 0;
                *addressBus = *dataBus;
                *dataBus = registers[instructionRegister & 0xff];
                RW = false;
                break;
            
            case 0x11:   // MOV INDIRECT, REGISTER
                switch(microcodePointer)
                {
                case 3:
                    *addressBus = *dataBus;
                    RW = true;
                    break;
                case 4:
                    *addressBus = *dataBus;
                    *dataBus = registers[instructionRegister & 0xff];
                    RW = false;
                    microcodePointer = 0;
                    break;
                }
                
                break;

            case 0x12:   // MOV REGISTER, ADDRESS
                switch(microcodePointer)
                {
                case 3:
                    *addressBus = *dataBus;
                    RW = true;
                    break;
                case 4:
                    registers[instructionRegister & 0xff] = *dataBus;
                    microcodePointer = 0;
                    break;
                }
                break;

            case 0x13:   // MOV REGISTER, INDIRECT
                switch(microcodePointer)
                {
                case 3:
                    *addressBus = *dataBus;
                    RW = true;
                    break;
                case 4:
                    *addressBus = *dataBus;
                    RW = true;
                    break;
                case 5:
                    registers[instructionRegister & 0xff] = *dataBus;
                    microcodePointer = 0;
                    break;
                }
                break;
            
            case 0x14:   // MOV REGISTER, NUMBER
                registers[instructionRegister & 0xff] = *dataBus;
                microcodePointer = 0;
                break;

            case 0x15:   // MOV REGISTER, REGISTER
                registers[instructionRegister & 0xff] = registers[*dataBus & 0xff];
                microcodePointer = 0;
                break;

            case 0x16:   // MOV REGISTER, SP
                registers[instructionRegister & 0xff] = stackPointer;
                microcodePointer = 0;
                break;
            
            case 0x17:   // MOV SP, REGISTER
                stackPointer = registers[instructionRegister & 0xff];
                microcodePointer = 0;
                break;
            
            case 0x20:   // CMP REGISTER, NUMBER
                flags &= 0b0011;
                flags |= (int)(registers[instructionRegister & 0xff] - *dataBus == 0) << 2;
                flags |= (int)((registers[instructionRegister & 0xff] - *dataBus) > registers[instructionRegister & 0xff]) << 3;
                microcodePointer = 0;
                break;
            
            case 0x21:   // CMP REGISTER, ADDRESS
                switch(microcodePointer)
                {
                case 3:
                    *addressBus = *dataBus;
                    RW = true;
                    break;
                case 4:
                    flags &= 0b0011;
                    flags |= (int)(registers[instructionRegister & 0xff] - *dataBus == 0) << 2;
                    flags |= (int)(registers[instructionRegister & 0xff] - *dataBus > registers[instructionRegister & 0xff]) << 3;
                    microcodePointer = 0;
                    break;
                }
                break;
            
            case 0x22:   // CMP REGISTER, INDIRECT
                switch(microcodePointer)
                {
                case 3:
                    *addressBus = *dataBus;
                    RW = true;
                    break;
                case 4:
                    *addressBus = *dataBus;
                    RW = true;
                    break;
                case 5:
                    flags &= 0b0011;
                    flags |= (int)(registers[instructionRegister & 0xff] - *dataBus == 0) << 2;
                    flags |= (int)(registers[instructionRegister & 0xff] - *dataBus > registers[instructionRegister & 0xff]) << 3;
                    microcodePointer = 0;
                    break;
                }
                break;
            
            case 0x23:   // CMP REGISTER, REGISTER
                flags &= 0b0011;
                flags |= (int)(registers[instructionRegister & 0xff] - registers[*dataBus & 0xff] == 0) << 2;
                flags |= (int)(registers[instructionRegister & 0xff] - registers[*dataBus & 0xff] > registers[instructionRegister & 0xff]) << 3;
                microcodePointer = 0;
                break;

            case 0x30:   // JMP ADDRESS
                instructionPointer = *dataBus;
                microcodePointer = 0;
                break;

            case 0x31:   // BRE ADDRESS
                if((flags & 0b0100) >> 2 == 1) instructionPointer = *dataBus;
                microcodePointer = 0;
                break;

            case 0x32:   // BRN ADDRESS
                if((flags & 0b1000) >> 3 == 1) instructionPointer = *dataBus;
                microcodePointer = 0;
                break;

            case 0x33:   // BNQ ADDRESS
                if((flags & 0b0100) >> 2 == 0) instructionPointer = *dataBus;
                microcodePointer = 0;
                break;

            case 0x34:   // BRC ADDRESS
                if((flags & 0b0010) >> 1 == 1) instructionPointer = *dataBus;
                microcodePointer = 0;
                break;

            case 0x40:   // ADD REGISTER, REGISTER
                registers[instructionRegister & 0xff] += registers[*dataBus & 0xff];
                flags |= registers[instructionRegister & 0xff] < registers[*dataBus & 0xff] << 1;
                microcodePointer = 0;
                break;
            
            case 0x41:   // ADD REGISTER, NUMBER
                registers[instructionRegister & 0xff] += *dataBus;
                flags |= registers[instructionRegister & 0xff] < *dataBus << 1;
                microcodePointer = 0;
                break;
            
            case 0x42:   // SUB REGISTER, REGISTER
                {
                    uint16_t tmp = registers[instructionRegister & 0xff];
                    registers[instructionRegister & 0xff] -= registers[*dataBus & 0xff];
                    flags |= registers[instructionRegister & 0xff] > tmp << 1;
                    microcodePointer = 0;
                    break;
                }

            case 0x43:   // SUB REGISTER, NUMBER
                {
                    uint16_t tmp = registers[instructionRegister & 0xff];
                    registers[instructionRegister & 0xff] -= *dataBus;
                    flags |= registers[instructionRegister & 0xff] > tmp << 1;
                    microcodePointer = 0;
                    break;
                }
            
            case 0x44:   // AND REGISTER, REGISTER
                registers[instructionRegister & 0xff] &= registers[*dataBus & 0xff];
                microcodePointer = 0;
                break;

            case 0x45:   // AND REGISTER, NUMBER
                registers[instructionRegister & 0xff] &= *dataBus;
                microcodePointer = 0;
                break;

            case 0x46:   // EOR REGISTER, REGISTER
                registers[instructionRegister & 0xff] ^= registers[*dataBus & 0xff];
                microcodePointer = 0;
                break;

            case 0x47:   // EOR REGISTER, NUMBER
                registers[instructionRegister & 0xff] ^= *dataBus;
                microcodePointer = 0;
                break;

            case 0x48:   // ORR REGISTER, REGISTER
                registers[instructionRegister & 0xff] |= registers[*dataBus & 0xff];
                microcodePointer = 0;
                break;

            case 0x49:   // ORR REGISTER, NUMBER
                registers[instructionRegister & 0xff] |= *dataBus;
                microcodePointer = 0;
                break;

            case 0x4a:   // ROR REGISTER
                registers[instructionRegister & 0xff] /= 2;
                microcodePointer = 0;
                break;

            case 0x50:   // PSH REGISTER
                RW = false;
                *dataBus = registers[instructionRegister & 0xff];
                *addressBus = stackPointer--;
                if(stackPointer < 0x1000) std::cout << "Warning: Stack Overflow! Stack went below $1000";
                microcodePointer = 0;
                break;
            
            case 0x51:   // POP REGISTER
                switch(microcodePointer)
                {
                case 3:
                    *addressBus = ++stackPointer;
                    if(stackPointer > 0x1400) std::cout << "Warning: Stack Underflow! Stack went above $1400";
                    RW = true;
                    break;
                case 4:
                    registers[instructionRegister & 0xff] = *dataBus;
                    microcodePointer = 0;
                    break;
                }
                break;

            case 0x52:   // JSR ADDRESS
                {
                    RW = false;
                    uint16_t jmpLocation = *dataBus;
                    *dataBus = instructionPointer;
                    *addressBus = stackPointer;
                    stackPointer--;
                    if(stackPointer < 0x1000) std::cout << "Warning: Stack Overflow! Stack went below $1000";
                    instructionPointer = jmpLocation;
                    microcodePointer = 0;
                    break;
                }

            case 0x53:   // RET
                
                switch(microcodePointer)
                {
                case 3:
                    *addressBus = ++stackPointer;
                    if(stackPointer > 0x1400) std::cout << "Warning: Stack Underflow! Stack went above $1400";
                    RW = true;
                    break;
                case 4:
                    instructionPointer = *dataBus;
                    microcodePointer = 0;
                    break;
                }
                break;
            
            case 0x60:   // CLC
                flags &= 0b11111101;
                microcodePointer = 0;
                break;

            case 0x61:   // CLZ
                flags &= 0b11111011;
                microcodePointer = 0;
                break;

            case 0x62:   // CLN
                flags &= 0b11110111;
                microcodePointer = 0;
                break;
            }
        }
    }
};

class RAM
{
public:
    uint16_t* memory = new uint16_t[0xD700];
    uint16_t* dataBus;
    uint16_t* addressBus;

    RAM(uint16_t* datBus, uint16_t* addrBus)
    {
        for(int i = 0; i < 0xD700; i++)
        {
            memory[i] = 0;
        }
        addressBus = addrBus;
        dataBus = datBus;
    }

    void updateState(bool RW)
    {  
        if(*addressBus >= 0 && *addressBus <= 0xD6FF)
        {
            if(RW == true)
            {
                *dataBus = memory[*addressBus];
            } else
            {
                memory[*addressBus] = *dataBus;
            }
        }
    }
};

class VisualProcessor
{
public:
    uint16_t* charMemory = new uint16_t[2048];
    uint8_t*  videoMemory = new uint8_t[128000];
    uint8_t*  tmpVideoMemory = new uint8_t[128000];
    uint16_t* dataBus;
    uint16_t* addressBus;
    uint8_t* charSet = new uint8_t[0xff*8];

    


    VisualProcessor(uint16_t* datBus, uint16_t* addrBus)
    {
        
        FILE *charSetFile;
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

    void updateState(bool RW)
    {
        if(*addressBus >= 0xD800 && *addressBus <= 0xDFFF)
        {
            if(RW == true)  // will output if the processor is reading and will write when the processor is writing
            {
                *dataBus = charMemory[*addressBus - 0xD800];
            } else
            {
                charMemory[*addressBus - 0xD800] = *dataBus;
            }
        }
    }


    void displayContents(sf::RenderWindow *window)
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
                        } else
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

};

class ROM
{
public:
    uint16_t* memory = new uint16_t[8192];
    uint16_t* dataBus;
    uint16_t* addressBus;

    ROM(uint16_t* _dataBus, uint16_t* _addressBus)
    {
        FILE *romFile;                       // read contents of rom.bin into rom memory
        romFile = fopen("rom.bin", "rb");
        fread(memory, 1, 16384, romFile);
        fclose(romFile);
        for(int i = 0; i < 8192; i++)        // flip endianess from 0xBBAA to 0xAABB (little to big)
        {
            uint16_t tmp = memory[i] & 0x00ff;
            memory[i] = (tmp << 8) | (memory[i] >> 8);
        }
        addressBus = _addressBus;
        dataBus = _dataBus;
    }

    void updateState(bool RW)
    {  
        if(*addressBus >= 0xE000 && *addressBus <= 0xFFFF) // only read from ROM if the address is in the correct range
        {
            if(!RW)                                        // check if the CPU is writing and block it (ROM is read-only)
            {
                std::cout << "Warning, Attempt to write to Read-Only Memory at address: " << std::hex << *addressBus << std::endl;
            } else                                         // check if the CPU is writing and block it (ROM is read-only)
            {
                *dataBus = memory[*addressBus - 0xE000];
            }
            
        }
    }
};

class IO
{
public:
    uint16_t* memory   = new uint16_t[256];
    uint16_t* hardDisk = new uint16_t[16777216];
    uint8_t   lastChar = 0;
    uint16_t* dataBus;
    uint16_t* addressBus;

    IO(uint16_t* _dataBus, uint16_t* _addressBus)
    {
        FILE *hardDiskFile;                         // read contents of file into harddisk
        hardDiskFile = fopen("hardDisk.bin", "rb");
        fread(hardDisk, 2, 16777216, hardDiskFile);
        fclose(hardDiskFile);
        for(int i = 0; i < 256; i++)                // null out memory
        {
            memory[i] = 0;
        }

        addressBus = _addressBus;
        dataBus = _dataBus;
    }

    void updateState(bool RW, sf::RenderWindow *window)
    {  
        if(*addressBus >= 0xD700 && *addressBus <= 0xD7FF) // Make sure address is in the correct range
        {
            if(RW)                                         // Write to bus when CPU is reading and read from bus when CPU is writing
            {
                *dataBus = memory[*addressBus - 0xD700];
            } else
            {
                memory[*addressBus - 0xD700] = *dataBus;
            }
        }
        if(!memory[0x04])
        {
            hardDisk[(memory[0x01]<<16) + memory[0x02]] = memory[0x03];
        } else {
            memory[0x03] = hardDisk[(memory[0x01]<<16) + memory[0x02]];
        }
        sf::Event event;

        // while there are pending events...
        while ((*window).pollEvent(event))                 // do keypresses and update correct memory location
        {
            // check the type of the event...
            switch (event.type)
            {
                case sf::Event::TextEntered:
                    if (event.text.unicode < 128)
                    {
                        lastChar = event.text.unicode;
                        memory[0] = static_cast<unsigned char>(event.text.unicode); // set $D700 to the ASCII value of the current key pressed
                    }
                    break;
                
                case sf::Event::KeyReleased:                                        // null out $D700 when no key is pressed
                    memory[0] = 0x0000;
                    break;

                case sf::Event::Closed:                                             // make sure SFML closes when X in corner is pressed
                    FILE *hardDiskFile;                                             // copy hardDisk to a file for longterm storage when the program closes
                    hardDiskFile = fopen("hardDisk.bin", "wb");
                    fwrite(hardDisk, 2, 16777216, hardDiskFile);
                    fclose(hardDiskFile);
                    (*window).close();

                default:
                    break;
            }
        }
    }
};

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
    while (window.isOpen())
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

