# in_file = open(input("name of assembly program you would like to assemble: "), "rt")
# out_size = int(input("minimum size of file you want to output: "), base = 16)
# offset = int(input("offset into file outputted: "), base = 16)

in_file = open("program.asm", "rt")
out_size = 0x2000
offset = 0x0000


origin_location = 0x0000
labels = {}

Lines = in_file.readlines()

for i in range(len(Lines)):
    Lines[i] = Lines[i].split(";",1)[0]
Lines[-1] += '\n'
Lines = [x[:-1].strip() for x in Lines]

def token_identifier(token):
    if(token[0] == '$'):
        return 'ADDR'
    elif(token[:2].upper() == '0X'):
        return 'HEXINT'
    elif(token[0] == '[' and token[-1] == ']'):
        return 'INDADDR'
    elif(token[0].upper() == 'R'):
        if(token[1] in "0123456789"):
            return 'REG'
        else:
            return 'INVALID'
    elif(token[0] in "0123456789"):
        return 'DECINT'
    else:
        return 'INVALID'



while("" in Lines):
    Lines.remove("")




tokens = []

for line in Lines:
    token_list = [""]
    for char in line:
        if(char == " "):
            token_list.append("")
        else:
            if(char != ','):
                token_list[-1] += char
    tokens.append(token_list)


for line in tokens:
    count = 0
    for token in line:
        if(count == 1):
            if(line[0] == ".org"):
                origin_location = int(token[1:], base = 16)
        count += 1


addr = origin_location
for line in tokens:
    if(line[0][0] != ";" and line[0][0] != "." ):
        addr += 2
    if(line[0][0:4].upper() == "WORD"):
        addr -= 1
    for token in line:
        if(token[-1] == ":"):
            addr -= 2
            labels[token[:-1]] = addr
            

instructions = (
                "NOP", "HLT", "MOV", "CMP", "JMP", "BRE", "BRN",
                "BNQ", "BRC", "ADD", "SUB", "AND", "EOR", "ORR",
                "PSH", "POP", "JSR", "RET", "CLC", "CLZ", "CLN", 
                "ROR", "WORD"
               )


while True:
    flag = False
    for i in range(len(tokens)):
        if(not(tokens[i][0].upper() in instructions)):
            tokens.pop(i)
            break
        if(i == range(len(tokens))[-1]):
            flag = True
    if(flag == True):
        break

output_bytes = []
inc = 0


for line in tokens:
    inc += 1
    inst = line[0]
    if(inst.upper() == "NOP"):
        output_bytes += [0x00,0x00,0x00,0x00]

    elif(inst.upper() == "HLT"):
        output_bytes += [0x01,0x00,0x00,0x00]

    elif(inst.upper() == "MOV"):
        MOV_opcodes = [0x10, 0x11, 0x12, 0x13, 0x14, 0x15]
        if( token_identifier(line[1]) == 'ADDR'):
            output_bytes += [0x10, int(line[2][1:])]
            output_bytes += [int(line[1][1:3], base = 16), int(line[1][3:5], base = 16)]
        elif(token_identifier(line[1]) == 'INDADDR'):
            output_bytes += [0x11, int(line[2][1:])]
            output_bytes += [int(line[1][2:4], base = 16), int(line[1][4:6], base = 16)]
        elif(token_identifier(line[2]) == 'ADDR'):
            output_bytes += [0x12, int(line[1][1:])]
            output_bytes += [int(line[2][1:3], base = 16), int(line[2][3:5], base = 16)]
        elif(token_identifier(line[2]) == 'INDADDR'):
            output_bytes += [0x13, int(line[1][1:])]
            output_bytes += [int(line[2][2:4], base = 16), int(line[2][4:6], base = 16)]
        elif(token_identifier(line[2]) == 'HEXINT' or token_identifier(line[2]) == 'DECINT'):
            output_bytes += [0x14, int(line[1][1:])]
            if(token_identifier(line[2]) == 'HEXINT'):
                output_bytes += [int(line[2], base = 16) >> 8, int(line[2], base = 16) & 0xff]
            else:
                output_bytes += [int(line[2]) >> 8, int(line[2]) & 0xff]
        elif(token_identifier(line[1]) == token_identifier(line[2]) == 'REG'):
            output_bytes += [0x15, int(line[1][1:]), 0x00, int(line[2][1:])]
        else:
            output_bytes += [0x14, int(line[1][1:]), labels[line[2]] >> 8, labels[line[2]] & 0xff ]

    elif(inst.upper() == "CMP"):
        if(token_identifier(line[2]) == 'HEXINT' or token_identifier(line[2]) == 'DECINT'):
            output_bytes += [0x20, int(line[1][1:])]
            if(token_identifier(line[2]) == 'HEXINT'):
                output_bytes += [int(line[2], base = 16) >> 8, int(line[2], base = 16) & 0xff]
            else:
                output_bytes += [int(line[2]) >> 8, int(line[2]) & 0xff]    
        elif(token_identifier(line[2]) == 'ADDR'):
            output_bytes += [0x21, int(line[1][1:])]
            output_bytes += [int(line[2][1:3], base = 16), int(line[2][3:5], base = 16)]
        elif(token_identifier(line[2]) == 'INDADDR'):
            output_bytes += [0x22, int(line[1][1:])]
            output_bytes += [int(line[2][2:4], base = 16), int(line[2][4:6], base = 16)]
        elif(token_identifier(line[2]) == 'REG'):
            output_bytes += [0x23, int(line[1][1:]), 0x00, int(line[2][1:])]
        else:
            output_bytes += [0x21, int(line[1][1:]), labels[line[2]] >> 8, labels[line[2]] & 0xff ]

    elif(inst.upper() == "JMP"):
        output_bytes += [0x30, 0x00] 
        if(token_identifier(line[1]) == 'INVALID'):
            output_bytes += [labels[line[1]] >> 8, labels[line[1]] & 0xff ]
        else:
            output_bytes += [int(line[1][1:3], base = 16), int(line[1][3:5], base = 16)]

    elif(inst.upper() == "BRE"):
        output_bytes += [0x31, 0x00] 
        if(token_identifier(line[1]) == 'INVALID'):
            output_bytes += [labels[line[1]] >> 8, labels[line[1]] & 0xff ]
        else:
            output_bytes += [int(line[1][1:3], base = 16), int(line[1][3:5], base = 16)]

    elif(inst.upper() == "BRN"):
        output_bytes += [0x32, 0x00] 
        if(token_identifier(line[1]) == 'INVALID'):
            output_bytes += [labels[line[1]] >> 8, labels[line[1]] & 0xff ]
        else:
            output_bytes += [int(line[1][1:3], base = 16), int(line[1][3:5], base = 16)]

    elif(inst.upper() == "BNQ"):
        output_bytes += [0x33, 0x00] 
        if(token_identifier(line[1]) == 'INVALID'):
            output_bytes += [labels[line[1]] >> 8, labels[line[1]] & 0xff ]
        else:
            output_bytes += [int(line[1][1:3], base = 16), int(line[1][3:5], base = 16)]    

    elif(inst.upper() == "BRC"):
        output_bytes += [0x34, 0x00] 
        if(token_identifier(line[1]) == 'INVALID'):
            output_bytes += [labels[line[1]] >> 8, labels[line[1]] & 0xff ]
        else:
            output_bytes += [int(line[1][1:3], base = 16), int(line[1][3:5], base = 16)]

    

    elif(inst.upper() == "ADD"):
        if(token_identifier(line[2]) == 'REG'):
            output_bytes += [0x40, int(line[1][1:]), 0x00, int(line[2][1:])]
        elif(token_identifier(line[2]) == 'HEXINT'):
            output_bytes += [0x41, int(line[1][1:]), int(line[2][2:4], base = 16), int(line[2][4:6], base = 16)]
        elif(token_identifier(line[2]) == 'DECINT'):
            output_bytes += [0x41, int(line[1][1:]), int(line[2]) >> 8, int(line[2]) & 0xff]       

    elif(inst.upper() == "SUB"):
        if(token_identifier(line[2]) == 'REG'):
            output_bytes += [0x42, int(line[1][1:]), 0x00, int(line[2][1:])]
        elif(token_identifier(line[2]) == 'HEXINT'):
            output_bytes += [0x43, int(line[1][1:]), int(line[2][2:4], base = 16), int(line[2][4:6], base = 16)]
        elif(token_identifier(line[2]) == 'DECINT'):
            output_bytes += [0x43, int(line[1][1:]), int(line[2]) >> 8, int(line[2]) & 0xff]
    
    elif(inst.upper() == "AND"):
        if(token_identifier(line[2]) == 'REG'):
            output_bytes += [0x44, int(line[1][1:]), 0x00, int(line[2][1:])]
        elif(token_identifier(line[2]) == 'HEXINT'):
            output_bytes += [0x45, int(line[1][1:]), int(line[2][2:4], base = 16), int(line[2][4:6], base = 16)]
        elif(token_identifier(line[2]) == 'DECINT'):
            output_bytes += [0x45, int(line[1][1:]), int(line[2]) >> 8, int(line[2]) & 0xff]
                
    elif(inst.upper() == "EOR"):
        if(token_identifier(line[2]) == 'REG'):
            output_bytes += [0x46, int(line[1][1:]), 0x00, int(line[2][1:])]
        elif(token_identifier(line[2]) == 'HEXINT'):
            output_bytes += [0x47, int(line[1][1:]), int(line[2][2:4], base = 16), int(line[2][4:6], base = 16)]
        elif(token_identifier(line[2]) == 'DECINT'):
            output_bytes += [0x47, int(line[1][1:]), int(line[2]) >> 8, int(line[2]) & 0xff]

    elif(inst.upper() == "ORR"):
        if(token_identifier(line[2]) == 'REG'):
            output_bytes += [0x48, int(line[1][1:]), 0x00, int(line[2][1:])]
        elif(token_identifier(line[2]) == 'HEXINT'):
            output_bytes += [0x49, int(line[1][1:]), int(line[2][2:4], base = 16), int(line[2][4:6], base = 16)]
        elif(token_identifier(line[2]) == 'DECINT'):
            output_bytes += [0x49, int(line[1][1:]), int(line[2]) >> 8, int(line[2]) & 0xff]

    elif(inst.upper() == "ROR"):
        output_bytes += [0x4a, int(line[1][1:]), 0x00, 0x00]

    elif(inst.upper() == "PSH"):
        output_bytes += [0x50, int(line[1][1:]), 0x00, 0x00]

    elif(inst.upper() == "POP"):
        output_bytes += [0x51, int(line[1][1:]), 0x00, 0x00]

    elif(inst.upper() == "JSR"):
        output_bytes += [0x52, 0x00]
        if(token_identifier(line[1]) == 'INVALID'):
            output_bytes += [labels[line[1]] >> 8, labels[line[1]] & 0xff ]
        else:
            output_bytes += [int(line[1][1:3], base = 16), int(line[1][3:5], base = 16)]

    elif(inst.upper() == "RET"):
        output_bytes += [0x53, 0x00, 0x00, 0x00]
        
    elif(inst.upper() == "CLC"):
        output_bytes += [0x60, 0x00, 0x00, 0x00]

    elif(inst.upper() == "CLZ"):
        output_bytes += [0x61, 0x00, 0x00, 0x00]

    elif(inst.upper() == "CLN"):
        output_bytes += [0x62, 0x00, 0x00, 0x00]

    elif(inst.upper() == "WORD"):
        if(token_identifier(line[1]) == 'INVALID'):
            output_bytes += [labels[line[1]] >> 8, labels[line[1]] & 0xff ]
        else:
            output_bytes += [int(line[1][2:4], base = 16), int(line[1][4:6], base = 16)]

    else:
        print("[ERROR] Invalid token on line " + str(inc) + ".")


output_bytes = [0] * offset + output_bytes

output_bytes = output_bytes + [0] * (out_size*2 - len(output_bytes))

# with open(input("name of binary file to output: "), "wb") as out_file:
#     out_file.write(bytearray(output_bytes))

with open("rom.bin", "wb") as out_file:
    out_file.write(bytearray(output_bytes))






