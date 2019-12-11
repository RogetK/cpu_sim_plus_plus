.text
:setup
    ldi r0 #0
    ldi r1 #1
    ld r3 r0 :size
    subi r2 r3 #1
    ldi r4 #0

:outer
;r4 index r5 inner_index

    ldi r5 #0
    ldi r0 #0
    ldi r1 #1
    subi r2 r2 #1

:outernot
;here is a comment

    ld r6 r0 :list
    ld r7 r1 :list
    cmp r7 r6
    bgt :noswap

    sto r6 r1 :list
    sto r7 r0 :list
:noswap

    addi r0 r0 #1
    addi r1 r1 #1
    addi r5 r5 #1
    cmp r5 r2
    blt :outernot

    addi r4 r4 #1
    cmp r4 r3
    blt :outer


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


