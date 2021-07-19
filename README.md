# R16-A-Virtual-16-bit-computer
A virtualised 16-bit computer that emulates a processor, RAM, video output, and more that was all created by me.

# Address Space
The address space is as follows <br />
$0000 - $D6FF  RAM <br />
$D700 - $D7FF  I/O <br />
$D800 - $DFFF  Video <br />
$E000 - $FFFF  ROM <br />
 <br />
The address space of I/O is as follows <br />
$D700 : Current Key Pressed <br />
$D701 : Address for hard disk (upper 16 bits) <br />
$D702 : Address for hard disk (lower 16 bits) <br />
$D702 : Data for hard disk <br />
$D703 : R/W for hard disk (0 writing, 1-65535 reading) <br />
$D704 - $D7FF - Currently unused <br />

# Opcode Format

All instructions take 2 Words (4 Bytes) and are in the form of 0xaa 0xbb 0xcc 0xdd <br />
0xaa is the opcode <br />
0xbb is a register is one is being used (ignored in all other cases such as ADD R3, 0x3522) <br />
0xcc is data high byte <br />
0xdd is data low byte and second register is one is used (always when no data is neede such as MOV R5, R3) <br />

# Addressing Modes <br />
 <br />
NUM - A 16-Bit Number like 0x1337 or 47855 <br />
ADR - A value at a 16-Bit address like $18C6 <br />
IND - An indirect address; An address at an address [$0D12] <br />
REG - One of the 16 general purpose registers like R12 <br />
SP  - Stack Pointer. Not a real addressing mode but more a syntax in the assembler for 2 instructions, MOV REG, SP and MOV SP, REG (actual addressing mode would be INST REG) these simply are for moving the stack pointer in and out of registers <br />

The 9 types of addressing modes are as follows <br />

INST REG, REG - Instruction using 2 registers <br />
INST REG, NUM - Instruction using a register and a number <br />
INST REG, ADR - Instruction using a register and an address <br />
INST REG, IND - Instruction using a register and an indirect address <br />
INST REG      - Instruction using just on register <br />
INST ADR, REG - Instruction using an address and register (Note: INST ADR, REG is not the same as INST REG, ADR e.g MOV $0000, r0 will move r0 into $0000 while MOV r0, $0000 will move $0000 into r0) <br />
INST ADR      - Instruction using just and address (like JMP) <br />
INST IND, REG - Instruction using an indirect address and a register <br />
INST          - a standalone instruction (like CLC or NOP) <br />

# Opcodes and Assembler Instructions

OPCODE - ASM INST     - DESCRIPTION <br />
0x00   - NOP          - Does nothing for 3 cycles <br />
0x10   - MOV ADR, REG - All MOV instructions copy the second operand into the first <br />
0x11   - MOV IND, REG <br />
0x12   - MOV REG, ADR <br />
0x13   - MOV REG, IND <br />
0x14   - MOV REG, NUM <br />
0x15   - MOV REG, REG <br />
0x16   - MOV REG, SP <br />
0x17   - MOV SP, REG <br />
0x20   - CMP REG, NUM - Compares updates zero and negative flags based on the difference of the two operands (First minus Second) <br />
0x21   - CMP REG, ADR <br />
0x22   - CMP REG, IND <br />
0x23   - CMP REG, REG <br />

