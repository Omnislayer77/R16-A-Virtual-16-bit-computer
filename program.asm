.org $E000

jsr main

end:
jmp end

hexLookup:
word 0x0030 ; 0
word 0x0031
word 0x0032
word 0x0033
word 0x0034
word 0x0035
word 0x0036
word 0x0037
word 0x0038
word 0x0039 ; 9
word 0x0041 ; A
word 0x0042
word 0x0043
word 0x0044
word 0x0045
word 0x0046 ; F

backspace:
  psh r0
  mov r0, $0000
  sub r0, 1

  cmp r0, 0xd82f
  bnq backspaceResetPtr
  mov r0, 0xdfff
backspaceResetPtr: 
  mov $0000, r0
  mov r0, 0x0020
  mov [$0000], r0
  pop r0
ret

typeChar:              ; will print r0 to the screen in ASCII and increment *cursorPointer
  psh r0               ; save contents of r0 incase of use somewhere else
  psh r1               ; save contents of r1 for same reason
  cmp r0, $0001
  bre typeCharEnd      ; if(currentChar == *lastChar) jmp typeCharEnd else *lastChar = currentChar
  mov $0001, r0

  
  mov r1, r0           ; copy r0 into r1
  and r1, 0x00ff       ; remove the most sig. byte of r1 (the color of the char)
  cmp r1, 0x0000       ; if(currentChar == 0) jmp typeCharEnd

  bre typeCharEnd           

  cmp r1, 0x0008       ; if(currentChar != 0x08) jmp typeCharDefault 
  bnq typeCharDefault 

  jsr backspace
  jmp typeCharEnd
typeCharDefault:
  mov [$0000], r0      ; type the charater to the screen

  mov r0, $0000        ; r0 = *cursorPointer + 1
  add r0, 0x0001

  cmp r0, 0xe000       ; if(r0 == $E000) r0 = $d830
  bnq updateCharPointer
  mov r0, 0xd830 
updateCharPointer:
  mov $0000, r0        ; *cursorPointer = r0
typeCharEnd:
  pop r0               ; restore the original contents of r0
  pop r1               ; restore r1
ret

multiply:
  psh r1
  psh r2
  mov r2, r1

  multLoop:
    cmp r1, 0
    add r0, r2
    sub r1, 1
    bnq multLoop

  pop r2
  pop r1
ret

shiftRightNTimes:
  sub r2, 1
  ror r1
  cmp r2, 0
  bnq shiftRightNTimes
ret

indexArray:      ; r0 - address of array  r1 - index
  add r0, r1
  mov $0002, r0
  mov r0, [$0002]
ret

printNum:
  psh r1
  mov r1, r0
  psh r1

  mov r0, hexLookup


  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  and r1, 0x000f
  jsr indexArray
  orr r0, 0x1c00
  jsr typeChar
  mov r0, 0x0000
  jsr typeChar

  mov r0, hexLookup

  pop r1
  psh r1

  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  ror r1
  and r1, 0x000f
  jsr indexArray
  orr r0, 0x1c00
  jsr typeChar
  mov r0, 0x0000
  jsr typeChar
  
  mov r0, hexLookup
  
  pop r1
  psh r1

  ror r1
  ror r1
  ror r1
  ror r1
  and r1, 0x000f
  jsr indexArray
  orr r0, 0x1c00
  jsr typeChar
  mov r0, 0x0000
  jsr typeChar
  
  mov r0, hexLookup

  pop r1
  psh r1

  and r1, 0x000f
  jsr indexArray
  orr r0, 0x1c00
  jsr typeChar
  mov r0, 0x0000
  jsr typeChar
  
  mov r0, hexLookup
    
  pop r1
ret

main:
  mov r0, 0xd830 ; cursorPointer $0000
  mov $0000, r0

  loop:
    mov r0, $d700
    orr r0, 0x1c00
    jsr typeChar
    jmp loop

  ; mov r0, 0x394e
  ; jsr printNum
ret






