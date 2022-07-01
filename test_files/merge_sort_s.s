.global merge_sort_s
.global merge_s

@ r0 is arr
@ r1 is aux array
@ r2 is start index = 0
@ r3 is end index = len-1

merge_sort_s:
		sub sp, sp, #16
		str lr, [sp]

		str r2, [sp, #4]		@ preserve start
		str r3, [sp, #8]		@ preserve end
		
		cmp r2, r3				@ base case
		bge done

		sub r3, r3, r2
		lsr r3, #1 				@ mid = (start + end) /2
		add r3, r2, r3
		
		bl merge_sort_s
		
		add r2, r3, #1   		@ r2 = mid + 1
		ldr r3, [sp, #8]		@ r3 = end
		
		bl merge_sort_s
		
		ldr r2, [sp, #4]		@ restore r2 = start
		
		bl merge_s
		

done:
		ldr lr, [sp]

		add sp, sp, #16
		bx lr
