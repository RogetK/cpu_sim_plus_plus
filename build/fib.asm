.text
    ldi r0 #0
    ldi r1 #0
    ldi r2 r0 :size

    ldi r3 #0
    ldi r4 #1
    ldi r5 #1

:loop_fib


:exit
    halt
    nop
    nop

.data
:size
    100

:output
    0
