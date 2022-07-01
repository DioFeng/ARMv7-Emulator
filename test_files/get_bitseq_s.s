.global get_bitseq_s


@ r0 - num
@ r1 - start
@ r2 - end / mask

@ r3 - mask
@ r12 - (len - 1)

get_bitseq_s:
    sub r12, r2, r1     @ r12 = len - 1

    lsr r0, r0, r1      @ Move sequence to beginning of word

    mov r3, #1          @ Build mask from 0b1
    lsl r3, r3, #31     @ Shift to far left
    asr r3, r3, r12     @ ASR to right by (len - 1) bits
    rsb r12, r12, #31   @ Determine remaining shift amout
    lsr r3, r3, r12     @ LSR to populate upper bits with 0s
    and r0, r0, r3      @ Apply mask

    bx lr

