# R16-A-Virtual-16-bit-computer
A virtualised 16-bit computer that emulates a processor, RAM, video output, and more that was all created by me.

# Address Space
The address space is as follows
$0000 - $D6FF  RAM
$D700 - $D7FF  I/O
$D800 - $DFFF  Video
$E000 - $FFFF  ROM

The address space of I/O is as follows
$D700 : Current Key Pressed
$D701 : Address for hard disk
$D702 : Data for hard disk
$D703 : R/W for hard disk (0 writing, 1-65535 reading)
$D704 - $D7FF - Currently unused

# Opcode Format

All instructions take 2 Words (4 Bytes) and are in the form of 0xaa 0xbb 0xcc 0xdd
0xaa is the opcode
0xbb is a register is one is being used (ignored in all other cases such as ADD R3, 0x3522)
0xcc is data high byte
0xdd is data low byte and second register is one is used (always when no data is neede such as MOV R5, R3)

# Addressing Modes

NUM - A 16-Bit Number like 0x1337 or 47855
ADR - A value at a 16-Bit address like $18C6
IND - An indirect address; An address at an address [$0D12]
REG - One of the 16 general purpose registers like R12
SP  - Stack Pointer. Not a real addressing mode but more a syntax in the assembler for 2 instructions, MOV REG, SP and MOV SP, REG (actual addressing mode would be INST REG) these simply are for moving the stack pointer in and out of registers

The 9 types of addressing modes are

INST REG, REG - Instruction using 2 registers
INST REG, NUM - Instruction using a register and a number
INST REG, ADR - Instruction using a register and an address
INST REG, IND - Instruction using a register and an indirect address
INST REG      - Instruction using just on register
INST ADR, REG - Instruction using an address and register (Note: INST ADR, REG is not the same as INST REG, ADR e.g MOV $0000, r0 will move r0 into $0000 while MOV r0, $0000 will move $0000 into r0)
INST ADR      - Instruction using just and address (like JMP)
INST IND, REG - Instruction using an indirect address and a register
INST          - a standalone instruction (like CLC or NOP)

# Opcodes and Assembler Instructions

OPCODE - ASM INST     - DESCRIPTION
0x00   - NOP          - Does nothing for 3 cycles
0x10   - MOV ADR, REG - All MOV instructions copy the second operand into the first
0x11   - MOV IND, REG 
0x12   - MOV REG, ADR
0x13   - MOV REG, IND
0x14   - MOV REG, NUM
0x15   - MOV REG, REG
0x16   - MOV REG, SP
0x17   - MOV SP, REG
0x20   - CMP REG, NUM - Compares updates zero and negative flags based on the difference of the two operands (First minus Second)
0x21   - CMP REG, ADR
0x22   - CMP REG, IND
0x23   - CMP REG, REG

