.global quadratic_s

@ r0 = x
@ r1 = a
@ r2 = b
@ r3 = c
quadratic_s:
    mul r2, r0, r2  @ bx
    mul r0, r0, r0  @ x*x
    mul r0, r1, r0  @ a*x*x
    add r0, r2, r0  @ + bx
    add r0, r3, r0  @ + c
    bx lr
