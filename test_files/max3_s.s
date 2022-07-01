@ max3_s.s - find the maximum value of three arguments using max2

.global max3_s

/* r0 - v0
 * r1 - v1
 * r2 - v2
 */
max3_s:
    cmp r0, r1
    bgt max2        @ if r0 is already the greater of the first two
    mov r0, r1      @ r1 is the greater of the first two
max2:
    cmp r0, r2      @ compare the greater of the first two with the third
    bgt done
    mov r0, r2      @ the third is the larger of all three
done:
    bx lr
