.text
    ldi r0 #0
    ldi r1 #10
    ldi r2 #11
    add r3 r2 r0
    sto r4 r3 r2
    sto r2 r0 :z
    halt

.data
:z
	0
	0
	0
;This is a test comment
