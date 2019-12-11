.text
    ldi r0 #0
    ld r1 r0 :size
:loop_start
    ld r2 r0 :x
    ld r3 r0 :y

    add r4 r2 r3
    sto r4 r0 :z
    addi r0 r0 #1
    cmp r0 r1

    blt :loop_start
    nop
    nop
    nop
    halt

.data
:size
    5
:x
    2
	4
	7
	9
	2
:y
	3
	5
	8
	10
	6
:z
	0
	0
	0
	0
	0
;This is a test comment
