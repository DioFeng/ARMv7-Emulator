
.global sort_s

@ r0 is the array
@ r1 is the len

sort_s:
		sub sp, sp, #128			@ allocate spaces for sp
		
		@ allocate and preserve space for regs
		str r0, [sp]
		str lr, [sp, #4]	
		str r4, [sp, #8]	
		str r5, [sp, #16]
		str r6, [sp, #32]
		str r1, [sp, #64]
		str r2, [sp, #68]

		mov r2, #0				@ r2 is start, i = 0

sort_loop:
		cmp r2, r1				@ if i == len
		beq done_loop			@ exit loop

		str r2, [sp, #68]		@ restore r2 = i
		sub r1, r1, r2			@ r1 = len - i

		lsl r2, #2
		add r0, r0, r2
		
		@add r0, r0, r2, lsl #2	@ &arr[i], subarray
		
		bl find_max_index_s		@ r0 = max_index

		ldr r1, [sp, #64]		@ restore r1
		ldr r2, [sp, #68]		@ restore r2
		
		mov r6, r0				@ r6 = max_index
		add r6, r6, r2			@ r6 = idx = max + i

		ldr r0, [sp]			@ restore array
		ldr r5, [r0, r2, lsl #2]		@ r5 = arr[i]
		ldr r4, [r0, r6, lsl #2]		@ r4 = arr[max_index]

		cmp r4, r5				@ if r4 > r5
		bgt swap
		
		add r2, r2, #1 			@ i += 1
		
		b sort_loop

swap:
		str r4, [r0, r2, lsl #2]		@ arr[i] = r5, r2 = i
		str r5, [r0, r6, lsl #2]		@ arr[max_index] = r5, r6 = max_index
		
		b sort_loop

done_loop:
								@ restore regs
		ldr r1, [sp, #64]
		ldr r6, [sp, #32]
		ldr r5, [sp, #16]
		ldr r4, [sp, #8]
		ldr lr, [sp, #4]
								@ free sp
		add sp, sp, #128

		bx lr
