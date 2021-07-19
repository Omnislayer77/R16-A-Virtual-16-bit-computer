.org $E000
; First V2.0 Code!

mov r0, 0x1000 ; move stack to $1000
mov sp, r0

writeDisk:
  psh r15
  mov r15, sp
  mov r1, r15
  add r1, 3
ret


psh 0x0000
psh 0x0002
psh 0x1337
jsr writeDisk

psh 0x0000
psh 0x0001
psh 0x1c2f
jsr writeDisk

loop:
jmp loop