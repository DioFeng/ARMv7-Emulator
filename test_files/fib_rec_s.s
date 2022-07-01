.global fib_rec_s

@r0 = n (index of fib sequence)
fib_rec_s:
    sub sp, sp, #8      @ preserve sp
    str lr, [sp]
    cmp r0, #1          @ if n <= 1
    ble done
    sub r0, r0, #1      @ set up n-1 in r0
    str r0, [sp, #4]    @ preserve n-1
    bl fib_rec_s        @ recurse(n-1)
    mov r1, r0          @ save retval in r1
    ldr r0, [sp, #4]    @ restore n-1
    sub r0, r0, #1      @ set up n-2 in r0
    str r1, [sp, #4]    @ preserve retval of first recursion
    bl fib_rec_s        @ recurse(n-2)
    ldr r1, [sp, #4]    @ restore retval of first recursion
    add r0, r0, r1      @ set up retval and fall thru
done:
    ldr lr, [sp]        @ restore sp
    add sp, sp, #8
    bx lr
    
