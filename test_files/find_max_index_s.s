.global find_max_index_s

@ r0 is the array
@ r1 is the length of the array

find_max_index_s:
		sub sp, sp, #8			@ allocate 8 bytes for sp
		str r4, [sp]			@ preserve r4

		mov ip, #0				@ ip = counter = 0
		mov r2, #0				@ set default max value at r2 to 0
		mov r4, #0				@ set default max index to 0
		
		

loop:
		cmp ip, r1					@ reach the end?
		beq loop_done			
		
		ldr r3, [r0, ip, lsl #2]	@ arr[i] 		
		cmp r3, r2					@ if r3 > r2
		bgt found_max
		
		add ip, ip, #1
		
		b loop

loop_done:
		mov r0, r4				@ return max index
		
		ldr r4, [sp]			@ restore r4
		
		add sp, sp, #8			@ dealloc sp		
		bx lr

found_max:
		mov r2, r3				@ set r2 a new max value from r3, r2 = r3
		mov r4, ip				@ set new max index to r4
		@add ip, ip, #1
		b loop
