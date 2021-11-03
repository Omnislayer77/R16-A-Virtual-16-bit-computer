# R16-A-Virtual-16-bit-computer
A virtualised 16-bit computer that emulates a processor, RAM, video output, and more that was all created by me.

# Address Space
### The address space is as follows
| Device | Range  |
|--|--|
| RAM | $0000 - $CEFF |
| I/O | $CF00 - $CFFF
| Video | $D000 - $DFFF
| ROM | $E000 - $FFFF

### The address space of I/O is as follows
| Address |  |
|--|--|
| $CF00 | Current Key Pressed  |
|$CF01|Address for hard disk (upper 16 bits) (currently not working)
|$CF02| Address for hard disk (lower 16 bits) (currently not working)
|$CF03| Data for hard disk (currently not working)
|$CF04|R/W for hard disk (0 writing, >0 reading) (currently not working) 
|$CF05 - $CFFF|Currently unused (Will act as normal RAM, but may be used in the future so be warned)



# Addressing Modes
.
### Single Addressing Modes
There are 6 different addressing modes which can be combined for multiple operand instructions.
 1. REG
 2. [REG]
 3. [REG + VAL]
 4. VAL
 5. ADDR
 6. [ADDR]
 
### Combonations of Addressing Modes
Much like x86, instructions with both operand reffering to memory locations are not present. This leaves us with 21 valid combonations of addressing modes for 0, 1, and 2 operand instructions. Every instruction can be catagorized in one of these 21 combonations.
 1. INST
 2. INST REG
 3. INST [REG]
 4. INST [REG + VAL]
 5. INST VAL
 6. INST ADDR
 7. INST [ADDR]
 8. INST REG, REG
 9.  INST REG, [REG]
 10. INST REG, [REG + VAL]
 11. INST REG, VAL
 12. INST REG, ADDR
 13. INST REG, [ADDR]
 14. INST [REG], REG 
 15. INST [REG], VAL
 16. INST [REG + VAL], REG
 17. INST [REG + VAL], VAL
 18. INST ADDR, REG
 19. INST ADDR, VAL
 20. INST [ADDR], REG
 21. INST [ADDR], VAL 
 
# Machine Code Format
For a full list of instructions and their respective opcode, go [here](https://docs.google.com/document/d/1ns5I-qCbmX3Ec4WrGWVTEZ7ZqJ5IhnKenzHa4Ed42-w/edit?usp=sharing). The machine code is assembled in the order of how the instruction would be written in assembly. Here is a list of each of the 6 addressing modes and how the would be written in machine code. Please note that values are stored in little endian form (least significant byte first).
| Name | Bytes  | 
|--|-- |
| REG  | 1 
| [REG]| 1
|[REG + VAL]| 3
|VAL| 2
|ADDR| 2
|[ADDR]| 2

**Examples**

| Example |Example code  
|--|--|
|r12| 0x0c
|[r9]| 0x09
|[r2 + 0xe131]| 0x02 0x31 0xe1
|0x5422|0x22 0x54
|$1299| 0x99 0x12
|[$0420]|0x20 0x04
|MOV [r3 + $a9e1], r6| 0x01 0x08 0x03 0xe1 0xa9 0x06 (note: MOV [REG + VAL], REG would have an opcode of 0x01 0x08)

### Opcodes
Opcodes can be one or two bytes long. For example, MOV REG, VAL would have the opcode ox 0x01 0x03. In this case all MOV opcodes start with 0x01 and the last byte is determined by what addressing modes the operands use. MOV had 14 valid combos as all of the MOV instructions have 2 operands. the lowest number, 8 in this case will be 0x00, 9 will be 0x01, etc. we see INST REG, VAL is number 11 which is 3 more than 8 so 0x03 is the second byte. Most if not all opcodes follow this pattern. Although, some similar opcodes (PUSH and POP, or JMP and conditionsal JMPs)) will have the same first byte. Again, a full list can be found [here](https://docs.google.com/document/d/1ns5I-qCbmX3Ec4WrGWVTEZ7ZqJ5IhnKenzHa4Ed42-w/edit?usp=sharing) as the 200+ opcodes is a lot to wirte in the readme file.


# Registers
The R16 CPU has 16 16-Bit General Purpose registers. They can also be used as 8-Bit registers in the high or low halves of the register. The registers are refered to as r0-r15 and take up one byte in operands. The lower nibble of the byte is the register number, and the higher nibble is zero for 16-Bit register, one for the high 8-Bit register, and two for the low 8-Bit register. r15 is used by the CPU as the Stack Pointer and assemblers should change SP to r15. r14 is used as a base pointer when doing function calls and is BP in assembly. r13 is used as the status flags register and is SF in assembler.  The Instruction Pointer is an independent register and starts out at 0xE000 when the processor starts as this is where ROM starts.


| Assembly |Machine code  |
|--|--|
| rY  16-Bit Register| 0x0Y 
| rYl 8-Bit Register| 0x2Y
| rYh 8-Bit Register| 0x1Y
