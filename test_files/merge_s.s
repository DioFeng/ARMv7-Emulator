.global merge_s

@ r0 is the arr
@ r1 is the aux array
@ r2 is the start index = 0 
@ r3 is the end index = len - 1

@ r4 = mid
@ r5 = ptr_l = start
@ r6 = ptr_r = mid + 1
@ r7 = loaded value arr[ptr_l]
@ r8 = loaded value arr[ptr_r]
@ r9 = idx to increment = start


merge_s:
		sub sp, sp, #36				@ allocate spaces for sp

		@ preserve regs
		str r2, [sp, #4]
		str r4, [sp, #12]
		str r5, [sp, #16]
		
		str r7, [sp, #24]
		str r8, [sp, #28]
		str r9, [sp, #32]
		
		add r4, r2, r3				@ r4 = start + end	
		lsr r4, #1					@ r4 = mid = len/2
		mov r5, r2					@ r5 = pt_l = start
		mov r9, r2					@ i = r2 = start
		
		mov r6, r4			
		add r6, r6, #1				@ r6 = pt_r = mid + 1
		str r6, [sp, #20]			@ preserve mid+1 to sp#20

		b merge_loop

merge_loop:
		cmp r5, r4 					@ while pt_l <= mid
		bgt cpy_rem_r				@ merge right hand side
		cmp r6, r3					@ && pt_r <= end
		bgt cpy_rem_l				@ merge left hand side
		
		ldr r7, [r0, r5, lsl #2]	@ arr[pt_l]
		ldr r8, [r0, r6, lsl #2]	@ arr[pt_r]

		cmp r7, r8
		ble cpy_l					@ if arr[pt_l] <= arr[pt_r]
		cmp r7, r8
		bgt cpy_r					@ if arr[pt_l] > arr[pt_r]

cpy_l:
		str r7, [r1, r9, lsl #2]
		add r5, #1 					@ pt_l++
		add r9, #1 					@ i++

		b merge_loop		
cpy_r:
		str r8, [r1, r9, lsl #2]
		add r6, #1 					@ pt_r++
		add r9, #1 					@ i++

		b merge_loop
		
cpy_rem_l:
		cmp r5, r4					@ while ptr_l <= mid
		bgt cpy_rem_r				@ if >, do the right

		@ copy remaining elements
		ldr r7, [r0, r5, lsl #2]
		str r7, [r1, r9, lsl #2]
		add r5, #1 					@ pt_l++
		add r9, #1 					@ i++
		b cpy_rem_l

cpy_rem_r:
		cmp r6, r3					@ while ptr_r <= end
		bgt merge_back				@ if >, finish merge

		@ copy the remaining elements
		ldr r8, [r0, r6, lsl #2]
		str r8, [r1, r9, lsl #2]
		add r6, #1 					@ pt_r++
		add r9, #1 					@ i++
		b cpy_rem_r

merge_back:
		ldr r2, [sp, #4]			@ set start index back to original
		b cpy_loop
cpy_loop:
		
		cmp r2, r3					@ while start <= end
		bgt loop_done
		
		@ copy elements from aux array to return array
		ldr r7, [r1, r2, lsl #2]
		str r7, [r0, r2, lsl #2]
		add r2, #1 					@ i++
		b cpy_loop

loop_done:
		@restore regs
			
		ldr r4, [sp, #12]			
		ldr r5, [sp, #16] 			
		ldr r6, [sp, #20]
		ldr r7, [sp, #24]
		ldr r8, [sp, #28]
		ldr r9, [sp, #32]
		add sp, sp, #36				@dealloc space for sp
		bx lr
