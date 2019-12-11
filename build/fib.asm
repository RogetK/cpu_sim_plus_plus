.text
    ldi r0 #0
;i r1 size r2
    ldi r1 #0
    ld r2 r0 :size

;r3 a r4 b r5 c
    ldi r3 #1
    ldi r4 #1
    ldi r5 #0

:fib
    add r5 r3 r4
    mov r3 r4
    mov r4 r5

    addi r1 r1 #1
    sto r5 r1 :output
    cmp r5 r2
    blt :fib

    j :return
    addi r0 r0 #23

:return
    halt
    nop
    nop

.data

:size
    500

:output
    0
