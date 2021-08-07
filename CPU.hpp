#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>

class CPU
{
public:
    uint16_t registers[16];         //16 general purpose registers
    uint16_t instructionPointer;    //Pointer that points to current instruction
    uint16_t stackPointer;          //1 Kb of stack between $1400 (top) and $1000
    uint16_t instructionRegister;   //Will hold the first word of the current instruction
    uint16_t microcodePointer;      //Current stage of the current instruction
    uint8_t flags;                  //Status flags in bit order of Negative, Zero, Carry, Halt ; 0b0000NZCH
    bool RW;                        //true if the CPU is reading and false if the CPU is writing

    uint16_t* dataBus;
    uint16_t* addressBus;

    CPU(uint16_t* datBus, uint16_t* addrBus);
    void updateState();
};

#endif /* CPU_HPP */
