.data
:list
    8
    7
    6
    5
    4
    3
    2
    1

.text
; stack pointer setup
    ldi r15 #1023
;load list size into r0
    ldi r0 #8
; load index into r1
    ldi r1 #0
;take first item from list into r2
    ld r2 r1 :list
;secondary index r3
    ldi r3 #1

:loop1
;load new element into r4
    ld r4 r3 :list
    cmp r4 r2
    bgt :skip_shift
:do_shift

    mov r5 r3
:loop2
    subi r7 r5 #1
    ld r6 r7 :list
    sto r6 r5 :list
    mov r5 r7
    cmp r7 r1
    bgt :loop2


    addi r1 r1 #1
:skip_shift
    addi r3 r3 #1


    nop
    nop
    nop
    nop
    halt
