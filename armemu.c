#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "armemu.h"

// Analysis functions

void analysis_init(struct analysis_st *ap) {
    ap->i_count = 0;
    ap->dp_count = 0;
    ap->mem_count = 0;
    ap->b_count = 0;
    ap->b_taken = 0;
    ap->b_not_taken = 0;
}

// Project04: Print results of dynamic analysis
void analysis_print(struct analysis_st *ap) {
    printf("=== Analysis\n");
    printf("I_count       = %d\n", ap->i_count);
    printf("DP_count      = %d (%.2f%%)\n", ap->dp_count,
           ((double) ap->dp_count / (double) ap->i_count) * 100.0);
    printf("SDT_count     = %d (%.2f%%)\n", ap->mem_count,
           ((double) ap->mem_count / (double) ap->i_count) * 100.0);    
    printf("B_count       = %d (%.2f%%)\n", ap->b_count,
           ((double) ap->b_count / (double) ap->i_count) * 100.0);
    printf("B_taken       = %d (%.2f%%)\n", ap->b_taken,
               ((double) ap->b_taken / (double) ap->b_count) * 100.0);
    printf("B_not_taken   = %d (%.2f%%)\n", ap->b_not_taken,
               ((double) ap->b_not_taken / (double) ap->b_count) * 100.0);
}

// Project04: Print results of dynamic analysis and cache sim
void armemu_print(struct arm_state *asp) {
    if (asp->analyze) {
        analysis_print(&(asp->analysis));
    }
    if (asp->cache_sim) {
        cache_print((&asp->cache));
    }
}

void armemu_init(struct arm_state *asp, uint32_t *func,
                 uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3) {
    int i;

    // Zero out registers
    for (i = 0; i < NREGS; i += 1) {
        asp->regs[i] = 0;
    }

    // Zero out the CPSR
    asp->cpsr = 0;

    // Zero out the stack
    for (i = 0; i < STACK_SIZE; i += 1) {
        asp->stack[i] = 0;
    }

    // Initialize the Program Counter
    asp->regs[PC] = (uint32_t) func;

    // Initialize the Link Register to a sentinel value
    asp->regs[LR] = 0;

    // Initialize Stack Pointer to the logical bottom of the stack
    asp->regs[SP] = (uint32_t) &asp->stack[STACK_SIZE];

    // Initialize the first 4 arguments in emulated r0-r3
    asp->regs[0] = a0;
    asp->regs[1] = a1;
    asp->regs[2] = a2;
    asp->regs[3] = a3;

    // Project04: Initialize dynamic analysis
    analysis_init(&asp->analysis);
    
    // Project04: Initialize cache simulator
    cache_init(&asp->cache);
}


bool armemu_is_bx(uint32_t iw) {
    uint32_t bxcode;

    bxcode = (iw >> 4) & 0xFFFFFF;

    // 0x12fff1 == 0b000100101111111111110001
    return bxcode == 0x12fff1;   
}

void armemu_bx(struct arm_state *asp, uint32_t iw) {
    uint32_t rn = iw & 0b1111;

    // Project04: increment dynamic analysis
    asp->analysis.b_count += 1;
    asp->analysis.b_taken += 1;    

    asp->regs[PC] = asp->regs[rn];
}

bool armemu_is_b(uint32_t iw) {
	
	uint32_t fixed = (iw >> 25) & 0b111;

	return (fixed == 0b101);	
}

bool armemu_is_bl(uint32_t iw) {
	uint32_t blcode = (iw >> 24) & 0b1;
	uint32_t fixed = (iw >> 25) & 0b111;

	return (blcode == 0b1) & (fixed == 0b101);	
}

/* b, bcc, bl
CPSR
flags:
   N egative	31
   Z ero		30
   C arry		29
   V overflow	28
*/


void armemu_branch(struct arm_state *asp, uint32_t iw) {
	uint32_t bcode = (iw >> 24) & 0b1;
	uint32_t N = (((asp->cpsr) >> 31) & 0b1);
	uint32_t Z = (((asp->cpsr) >> 30) & 0b1);
	uint32_t V = (((asp->cpsr) >> 28) & 0b1);
	uint32_t offset_extended = ((int32_t)(iw & 0xFFFFFF) << 8) >> 8;
	uint32_t offset_converted = offset_extended << 2;
	offset_converted += 8;
	
	asp->analysis.b_count += 1;
	uint32_t cond = ((iw >> 28) & 0b1111);
	if (!bcode) {
		//printf("Branch:\n");
		/*
		EQ
		NE
		GT
		LT
		GE
		LE
		*/
		if ((Z == 1 && (((iw >> 28) & 0b1111) == 0b0000)) ||
			((Z == 0 && (((iw >> 28) & 0b1111) == 0b0001))) ||
			((Z == 0 && N == V) && (((iw >> 28) & 0b1111) == 0b1100)) ||
			((N != V) && (((iw >> 28) & 0b1111) == 0b1011)) ||
			((N == V) && (((iw >> 28) & 0b1111) == 0b1010)) || 
			((N != V || Z == 1) && (((iw >> 28) & 0b1111) == 0b1101)) || cond == 0b1110
			) {
				//printf("BCC\n");
				//asp->regs[LR] = asp->regs[PC] + 4;
				asp->analysis.b_taken += 1;
				//asp->regs[LR] = asp->regs[PC] + 4;
				asp->regs[PC] += offset_converted;
			} else {
				//printf("Meh\n");
				asp->analysis.b_not_taken += 1;
				asp->regs[PC] = asp->regs[PC] + 4;
			}
	} else {
		//printf("BL:\n");
			asp->analysis.b_taken += 1;
			asp->regs[LR] = asp->regs[PC] + 4;
			asp->regs[PC] += offset_converted;
	}
	
	
}

bool armemu_is_shift(uint32_t iw) {
	uint32_t is_shifted = (iw >> 25) & 0b1;
	return is_shifted == 0b0;
}

void armemu_shift(struct arm_state *asp, uint32_t iw) {
	uint32_t shift = (iw >> 4) & 0b1;
	uint32_t shift_type = (iw >> 5) & 0b11;
	uint32_t rm = iw & 0b1111;
	uint32_t rd = (iw >> 12) & 0b1111;

	uint32_t shift_amount;

	if (shift) {
			uint32_t rs = (iw >> 8) & 0b1111;
			shift_amount = asp->regs[rs];
		} else {
			shift_amount = (iw >> 7) & 0b11111;
		}
		//printf("Shift amount: %u\n", shift_amount);
	
		if (shift_type == 0b00) {
			//printf("lsl\n");
			//printf("rm: %u, rn: %u\n",asp->regs[rm],asp->regs[rn] );
			asp->regs[rd] = (uint32_t)asp->regs[rm] << shift_amount;
		} else if (shift_type == 0b01) {
			//printf("lsr\n");
			//printf("rm: %u, rn: %u\n",asp->regs[rm],asp->regs[rn] );
			asp->regs[rd] = ((uint32_t)asp->regs[rm] >> shift_amount);
		} else if (shift_type == 0b10) {
			//printf("asr\n");
			//printf("rm: %u, rn: %u\n",asp->regs[rm],asp->regs[rn] );
			asp->regs[rd] = (int32_t)asp->regs[rm] >> shift_amount;
		}
			//printf("rd: %u\n", asp->regs[rd]);
			//printf("==========================\n");
}

bool armemu_is_sdt(uint32_t iw) {
	uint32_t sdt_bits = (iw >> 26) & 0b11;
    return sdt_bits == 0b01;
}

void armemu_sdt(struct arm_state *asp, uint32_t iw) {
	uint32_t i_bit = (iw >> 25) & 0b1;
	uint32_t b_bit = (iw >> 22) & 0b1;
	uint32_t l_bit = (iw >> 20) & 0b1;
	uint32_t rn = (iw >> 16) & 0b1111; //Base reg
	uint32_t rd = (iw >> 12) & 0b1111; //Source/Destination reg
	uint32_t rm = iw & 0b1111;
	
	uint32_t offset;
	uint32_t oper2;


	uint32_t shift = (iw >> 4) & 0b1;
	uint32_t shift_type = (iw >> 5) & 0b11;
	
	uint32_t shift_amount;
	
	

	// Project04: Increment analysis count
	asp->analysis.mem_count += 1;
	
	if (!i_bit) {
		offset = (iw & 0b111111111111); // 0
	} else {
		if (shift) {
			uint32_t rs = (iw >> 8) & 0b1111;
			shift_amount = asp->regs[rs];
		} else {
			shift_amount = (iw >> 7) & 0b11111;
		}
		//printf("Shift amount: %u\n", shift_amount);
	
		if (shift_type == 0b00) {
			//printf("lsl\n");
			//printf("rm: %u, rn: %u\n",asp->regs[rm],asp->regs[rn] );
			offset = (uint32_t)asp->regs[rm] << shift_amount;
		} else if (shift_type == 0b01) {
			//printf("lsr\n");
			//printf("rm: %u, rn: %u\n",asp->regs[rm],asp->regs[rn] );
			offset = ((uint32_t)asp->regs[rm] >> shift_amount);
		} else if (shift_type == 0b10) {
			//printf("asr\n");
			//printf("rm: %u, rn: %u\n",asp->regs[rm],asp->regs[rn] );
			offset = (int32_t)asp->regs[rm] >> shift_amount;
		}
		//offset = asp->regs[rm]; // 1
	}

	if (!b_bit) {
		oper2 = *((uint32_t*) (asp->regs[rn] + offset));
	} else {
		oper2 = *((uint8_t*) (asp->regs[rn] + offset));
	}

	if (!l_bit) {
		//printf("STR - v: %u, rd: %u\n", oper2, asp->regs[rd]);
		//printf("rd label: %u\n", rd);
		if (!b_bit) {
			*((uint32_t*) (asp->regs[rn] + offset)) = asp->regs[rd];
		} else {
			*((uint8_t*) (asp->regs[rn] + offset)) = asp->regs[rd];
		}
	} else {
		//printf("LDR - rd %u, oper2: %u\n", asp->regs[rd], oper2);
		//printf("rd label: %u\n", rd);
		asp->regs[rd] = oper2;
	}
	
	if (rd != PC) {
    	asp->regs[PC] = asp->regs[PC] + 4;
	}
	
}

bool is_dp(uint32_t iw) {
    uint32_t dp_bits = (iw >> 26) & 0b11;
    return dp_bits == 0b00;    
}

void handle_dp(struct arm_state *asp, uint32_t iw) {

	uint32_t opcode = (iw >> 21) & 0b1111;
	uint32_t i_bit = (iw >> 25) & 0b1;
	uint32_t rn = (iw >> 16) & 0b1111;
	uint32_t rd = (iw >> 12) & 0b1111;
	uint32_t rm = iw & 0b1111;
	uint32_t imm = iw & 0b11111111;
	uint32_t oper2;
	
	// Project04: Increment analysis count
	asp->analysis.dp_count += 1;

	if (!i_bit) {
	    oper2 = asp->regs[rm];
	} else {
	    oper2 = imm;
	}

	switch (opcode) {
		case 0b0000:
			//printf("AND: rm: %u & rn: %u\n", oper2, asp->regs[rn]);
			asp->regs[rd] = asp->regs[rn] & oper2; //And
			break;
		case 0b0010:
			//printf("SUB: rn: %u - rm: %u\n", asp->regs[rn], oper2);
			asp->regs[rd] = asp->regs[rn] - oper2; //Sub
			break;
		case 0b0011:
			//printf("RSB: rm: %u - rn: %u\n", oper2, asp->regs[rn] );
			asp->regs[rd] = oper2 - asp->regs[rn]; //Rsb
			break;
		case 0b0100:
			//printf("ADD: rn: %u + rm: %u\n", asp->regs[rn], oper2);
			asp->regs[rd] = asp->regs[rn] + oper2; //Add
			break;
		case 0b1101:
			//printf("Mov: rd: %u = rm: %u\n", (uint32_t)asp->regs[rd], oper2);
			if (armemu_is_shift(iw)) {
				armemu_shift(asp, iw);
			}
			else {
				asp->regs[rd] = oper2; //Mov
			}
			break;
		case 0b1010:
			//printf("Cmp: rn: %u = rm: %u\n", asp->regs[rn], oper2);
			asp->cpsr = 0;
			if ((int32_t)(asp->regs[rn] - oper2) == 0) {
				asp->cpsr |= (0 << 31);
				asp->cpsr |= (1 << 30);
				asp->cpsr |= (0 << 29);
				asp->cpsr |= (0 << 28);
			} else if ((int32_t)(asp->regs[rn] - oper2) > 0) {		
				asp->cpsr |= (1 << 31);
				asp->cpsr |= (0 << 30);
				asp->cpsr |= (0 << 29);
				asp->cpsr |= (1 << 28);				
			} else {
				asp->cpsr |= (1 << 31);
				asp->cpsr |= (0 << 30);
				asp->cpsr |= (1 << 29);
				asp->cpsr |= (0 << 28);
			}
			break;
		default:
			printf("Meh");
	}
	
	if (rd != PC) {
    	asp->regs[PC] = asp->regs[PC] + 4;
	}
}


bool armemu_is_mul(uint32_t iw) {
	uint32_t dp_bits = (iw >> 22) & 0b111111;
	uint32_t code = (iw >> 4) & 0b1111;
	return (dp_bits == 0b000000) & (code == 0b1001);
}

void armemu_mul(struct arm_state *asp, uint32_t iw) {
	// Mul -> Rd = Rm * Rs
	// A = 1/0 -> 0 MUL, 1 MLA
	// S = 1/0 -> 1 Set condition codes, 0 do nothing 
	uint32_t rm = iw & 0b1111;
	uint32_t rs = (iw >> 8) & 0b1111;
	uint32_t rn = 0b0000;
	uint32_t rd = (iw >> 16) & 0b1111;
	
	// Project04: Increment analysis count
	asp->analysis.dp_count += 1;
	    
	if (((iw >> 20) & 0b1) == 0b1) {
		rn = (iw >> 12) & 0b1111;
	}

	asp->regs[rd] = asp->regs[rm] * asp->regs[rs] + rn;

	if (rd != PC) {
		asp->regs[PC] = asp->regs[PC] + 4;
	}
	
}


void armemu_one(struct arm_state *asp) {

    asp->analysis.i_count += 1;

    /* Project04: get instruction word from instruction cache
       instead of the PC pointer directly
       uint32_t iw = *((uint32_t *) asp->regs[PC]);
    */
    uint32_t iw = cache_lookup(&asp->cache, asp->regs[PC]);

    // Order matters: more constrained to least constrained
    if (armemu_is_bx(iw)) {
		armemu_bx(asp, iw);
	} else if (armemu_is_mul(iw)) {
		//printf("mul\n");
		armemu_mul(asp, iw);
	} else if (armemu_is_b(iw)) {
		armemu_branch(asp, iw);
	} else if (is_dp(iw)) {
		handle_dp(asp, iw);
	} else if (armemu_is_sdt(iw)) {
		armemu_sdt(asp, iw);
	} else {
		printf("armemu_one() invalid instruction\n");
		exit(-1);
	}
}

int armemu(struct arm_state *asp) {
    while (asp->regs[PC] != 0) {
        armemu_one(asp);
    }
    
    return (int) asp->regs[0];
}
