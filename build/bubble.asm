.text
:setup
    ldi r0 #0
    ldi r1 #1
    ld r3 r0 :size
    subi r2 r3 #1

:out
;r4 index r5 inner_index
    ldi r4 #0
    ldi r5 #0
:inner
    ld r6 r0 :list
    ld r7 r1 :list
    cmp r6 r7
    bgt :swap
    ldi r15 #23
:swap
    addi r8 r6 #0
    mov r6 r7
    mov r7 r8
    sto r6 r0 :list
    sto r7 r1 :list
    addi r0 r0 #1
    addi r1 r1 #1
    addi r5 r5 #1
    cmp r5 r2
    blt :inner



:return
    nop
    nop
    nop
    halt


.data
:size
    5
:list
    5
    1
    7
    2
    4
