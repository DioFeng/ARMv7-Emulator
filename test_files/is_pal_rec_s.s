.global is_pal_rec_s

@r0 = str
@r1 = start
@r2 = end

is_pal_rec_s:
    sub sp, sp, #8          @ preserve lr
    str lr, [sp]
    cmp r1, r2              @ if start >= end
    bge pal_true
    ldrb r3, [r0, r1]       @ r3 is str[start]
    ldrb ip, [r0, r2]       @ ip is str[end]
    cmp r3, ip              @ if str[start] != str[end]
    bne pal_false
    add r1, r1, #1          @ start + 1
    sub r2, r2, #1          @ end - 1
    bl is_pal_rec_s            @ recurse
    b pal_done              @ leave return value in r0
pal_true:
    mov r0, #1              @ return true
    b pal_done
pal_false:
    mov r0, #0              @ return false, fall thru
pal_done:
    ldr lr, [sp]            @ restore lr
    add sp, sp, #8
    bx lr
