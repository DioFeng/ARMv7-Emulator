.global to_upper_s

@ r0 = buffer
to_upper_s:
    mov ip, #0          @ loop index i
loop:
    ldrb r1, [r0, ip]   @ r1 = arr[i] 
    cmp r1, #0          @ null terminator?
    beq done
    cmp r1, #97         @ less than 'a', go again
    blt next
    cmp r1, #122        @ greater than 'z', go again
    bgt next
    sub r1, r1, #32     @ in range 'a' ... 'z', make upper
    strb r1, [r0, ip]   @ store to buffer
next:
    add ip, ip, #1
    b loop
done:
    bx lr    
