.text
:fib
    ldi r0 #0
    ldi r1 #0
    ldi r2 #0
    ld r3 r0 :n

:fib
    NOP
    addi r2 r2 #1
:loop
    addi r2 r2 #1
    cmp r2 r3
    j :fib

:return
    halt
    nop
    nop

.data

:n
    10

:x
    0
