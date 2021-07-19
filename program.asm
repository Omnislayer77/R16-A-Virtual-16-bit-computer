.org $E000
; First V2.0 Code!

mov r0, 0x1000 ; move stack to $1000
mov sp, r0

; mov r0, 0x0000
; mov $d701, r0
; mov r0, 0x0002
; mov $d702, r0
; mov r0, 0x0000
; mov $d704, r0
; mov r0, 0xff34
; mov $0xd703, r0
; mov $d704, r0

; mov r0, 0x0000
; mov $d701, r0
; mov r0, 0x0007
; mov $d702, r0
; mov r0, 0x0000
; mov $d704, r0
; mov r0, 0x4399
; mov $0xd703, r0
; mov $d704, r0

mov r0, 0x0000
mov $d701, r0
mov r0, 0x0002
mov $d702, r0
mov $d704, r0
mov r0, $d703
mov $d841, r0


loop:
jmp loop