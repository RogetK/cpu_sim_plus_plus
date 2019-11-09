.text
    add r2 r0 r0
    divd r0 r3 r1
    mult r6 r0 r6
    sub r0 r0 r0
    ld r1 r0 :y
    ldi r5 #23
    cmp r0 r1
    sto r2 r0 :z
    halt

.data
:x
    1
	2
	3
:y
	4
	5
	6
:z
	0
	0
	0
;This is a test comment
