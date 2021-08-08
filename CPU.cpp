#include "CPU.hpp"

CPU::CPU(uint16_t* datBus, uint16_t* addrBus)
{
    addressBus = addrBus;
    dataBus = datBus;

    for(int i = 0; i < sizeof(registers)/sizeof(uint16_t); i++)
    {
        registers[i] = 0;
    }

    instructionPointer = 0xE000;
    stackPointer = 0x0000;
    flags = 0x00;
    RW = true;
    microcodePointer = 0x0000;
}

void CPU::updateState()
{
    microcodePointer++;

    if(microcodePointer == 1)
    {
        *addressBus = instructionPointer;
        RW = true;
        instructionPointer++;
    }

    else if(microcodePointer == 2)
    {
        instructionPointer = *dataBus;
        *addressBus = instructionPointer;
        instructionPointer++;
    }

    else
    {
        switch((instructionRegister & 0xFF00)>>8)
        {
            case 0x00: //NOP
            {
                microcodePointer = 0;
                break;
            }

            case 0x01: //HLT
            {
                microcodePointer = 0;
                flags |= 0x01;
                break;
            }

            case 0x10: //MOV ADDRESS, REGISTER
            {
                microcodePointer = 0;
                *addressBus = *dataBus;
                *dataBus = registers[instructionRegister & 0xFF];
                RW = false;
                break;
            }

            case 0x11: //MOV INDIRECT, REGISTER
            {
                switch(microcodePointer)
                {
                    case 3:
                    {
                        *addressBus = *dataBus;
                        RW = true;
                        break;
                    }

                    case 4:
                    {
                        *addressBus = *dataBus;
                        *dataBus = registers[instructionRegister & 0xFF];
                        RW = false;
                        microcodePointer = 0;
                        break;
                    }
                }

                break;
            }

            case 0x12: //MOV REGISTER, ADDRESS
            {
                switch(microcodePointer)
                {
                    case 3:
                    {
                        *addressBus = *dataBus;
                        RW = true;
                        break;
                    }

                    case 4:
                    {
                        registers[instructionRegister & 0xFF] = *dataBus;
                        microcodePointer = 0;
                        break;
                    }
                }

                break;
            }

            case 0x13: //MOV REGISTER, INDIRECT
            {
                switch(microcodePointer)
                {
                    case 3:
                    {
                        *addressBus = *dataBus;
                        RW = true;
                        break;
                    }

                    case 4:
                    {
                        *addressBus = *dataBus;
                        RW = true;
                        break;
                    }

                    case 5:
                    {
                        registers[instructionRegister & 0xFF] = *dataBus;
                        microcodePointer = 0;
                        break;
                    }
                }

                break;
            }

            case 0x14: //MOV REGISTER, NUMBER
            {
                registers[instructionRegister & 0xFF] = *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x15: //MOV REGISTER, REGISTER
            {
                registers[instructionRegister & 0xFF] = registers[*dataBus & 0xFF];
                microcodePointer = 0;
                break;
            }

            case 0x16: //MOV REGISTER, SP
            {
                registers[instructionRegister & 0xFF] = stackPointer;
                microcodePointer = 0;
                break;
            }

            case 0x17: //MOV SP, REGISTER
            {
                stackPointer = registers[instructionRegister & 0xFF];
                microcodePointer = 0;
                break;
            }

            case 0x20: //CMP REGISTER, NUMBER
            {
                flags &= 0b0011;
                flags |= (int)(registers[instructionRegister & 0xFF] - *dataBus == 0) << 2;
                flags |= (int)((registers[instructionRegister & 0xFF] - *dataBus) > registers[instructionRegister & 0xFF]) << 3;
                microcodePointer = 0;
                break;
            }

            case 0x21: //CMP REGISTER, ADDRESS
            {
                switch(microcodePointer)
                {
                    case 3:
                    {
                        *addressBus = *dataBus;
                        RW = true;
                        break;
                    }

                    case 4:
                    {
                        flags &= 0b0011;
                        flags |= (int)(registers[instructionRegister & 0xFF] - *dataBus == 0) << 2;
                        flags |= (int)(registers[instructionRegister & 0xFF] - *dataBus > registers[instructionRegister & 0xFF]) << 3;
                        microcodePointer = 0;
                        break;
                    }
                }

                break;
            }

            case 0x22: //CMP REGISTER, INDIRECT
            {
                switch(microcodePointer)
                {
                    case 3:
                    {
                        *addressBus = *dataBus;
                        RW = true;
                        break;
                    }

                    case 4:
                    {
                        *addressBus = *dataBus;
                        RW = true;
                        break;
                    }

                    case 5:
                    {
                        flags &= 0b0011;
                        flags |= (int)(registers[instructionRegister & 0xFF] - *dataBus == 0) << 2;
                        flags |= (int)(registers[instructionRegister & 0xFF] - *dataBus > registers[instructionRegister & 0xFF]) << 3;
                        microcodePointer = 0;
                        break;
                    }
                }

                break;
            }

            case 0x23: //CMP REGISTER, REGISTER
            {
                flags &= 0b0011;
                flags |= (int)(registers[instructionRegister & 0xFF] - *dataBus == 0) << 2;
                flags |= (int)(registers[instructionRegister & 0xFF] - *dataBus > registers[instructionRegister & 0xFF]) << 3;
                microcodePointer = 0;
                break;
            }

            case 0x30: //JMP ADDRESS
            {
                instructionPointer = *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x31: //BRE ADDRESS
            {
                if((flags & 0b0100) >> 2 == 1) instructionPointer = *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x32: //BRN ADDRESS
            {
                if((flags & 0b1000) >> 3 == 1) instructionPointer = *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x33: //BNQ ADDRESS
            {
                if((flags & 0b0100) >> 2 == 0) instructionPointer = *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x34: //BRC ADDRESS
            {
                if((flags & 0b0010) >> 1 == 1) instructionPointer = *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x40: //ADD REGISTER, REGISTER
            {
                registers[instructionRegister & 0xFF] += registers[*dataBus & 0xFF];
                flags |= registers[instructionRegister & 0xFF] < registers[*dataBus & 0xFF] << 1;
                microcodePointer = 0;
                break;
            }

            case 0x41: //ADD REGISTER, NUMBER
            {
                registers[instructionRegister & 0xFF] += *dataBus;
                flags |= registers[instructionRegister & 0xFF] < *dataBus << 1;
                microcodePointer = 0;
                break;
            }

            case 0x42: //SUB REGISTER, REGISTER
            {
                uint16_t tmp = registers[instructionRegister & 0xFF];
                registers[instructionRegister & 0xFF] -= registers[*dataBus & 0xFF];
                flags |= registers[instructionRegister & 0xFF] > tmp << 1;
                microcodePointer = 0;
                break;
            }

            case 0x43: //SUB REGISTER, NUMBER
            {
                uint16_t tmp = registers[instructionRegister & 0xFF];
                registers[instructionRegister & 0xFF] -= *dataBus;
                flags |= registers[instructionRegister & 0xFF] > tmp << 1;
                microcodePointer = 0;
                break;
            }

            case 0x44: //AND REGISTER, REGISTER
            {
                registers[instructionRegister & 0xFF] &= registers[*dataBus & 0xFF];
                microcodePointer = 0;
                break;
            }

            case 0x45: //AND REGISTER, NUMBER
            {
                registers[instructionRegister & 0xFF] &= *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x46: //EOR REGISTER, REGISTER
            {
                registers[instructionRegister & 0xFF] ^= registers[*dataBus & 0xFF];
                microcodePointer = 0;
                break;
            }

            case 0x47: //EOR REGISTER, NUMBER
            {
                registers[instructionRegister & 0xFF] ^= *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x48: //ORR REGISTER, REGISTER
            {
                registers[instructionRegister & 0xFF] |= registers[*dataBus & 0xFF];
                microcodePointer = 0;
                break;
            }

            case 0x49: //ORR REGISTER, NUMBER
            {
                registers[instructionRegister & 0xFF] |= *dataBus;
                microcodePointer = 0;
                break;
            }

            case 0x4A: //ROR REGISTER
            {
                registers[instructionRegister & 0xFF] /= 2;
                microcodePointer = 0;
                break;
            }

            case 0x50: //PSH REGISTER
            {
                RW = false;
                *dataBus = registers[instructionRegister & 0xFF];
                *addressBus = stackPointer--;
                microcodePointer = 0;
                break;
            }

            case 0x51: //POP REGISTER
            {
                switch(microcodePointer)
                {
                    case 3:
                    {
                        *addressBus = ++stackPointer;
                        RW = true;
                        break;
                    }

                    case 4:
                    {
                        registers[instructionRegister & 0xFF] = *dataBus;
                        microcodePointer = 0;
                        break;
                    }
                }

                break;
            }

            case 0x52: //JSR ADDRESS
            {
                RW = false;
                uint16_t jmpLocation = *dataBus;
                *dataBus = instructionPointer;
                *addressBus = stackPointer;
                stackPointer--;
                instructionPointer = jmpLocation;
                microcodePointer = 0;
                break;
            }

            case 0x53: //RET
            {
                switch(microcodePointer)
                {
                    case 3:
                    {
                        *addressBus = ++stackPointer;
                        RW = true;
                        break;
                    }

                    case 4:
                    {
                        instructionPointer = *dataBus;
                        microcodePointer = 0;
                        break;
                    }
                }

                break;
            }

            case 0x60: //CLC
            {
                flags &= 0b11111101;
                microcodePointer = 0;
                break;
            }

            case 0x61: //CLZ
            {
                flags &= 0b11111011;
                microcodePointer = 0;
                break;
            }

            case 0x62: //CLN
            {
                flags &= 0b11110111;
                microcodePointer = 0;
                break;
            }
        }
    }
}
