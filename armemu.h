#include <stdint.h>
#include <stdbool.h>


#define ARMEMU_DEBUG 0
#define MAX_INSTRUCTION_COUNT 1000000
#define MAX_ARRAY 1024

#define NREGS 16
#define STACK_SIZE 4096
#define SP 13
#define LR 14
#define PC 15

#define CPSR_N 31
#define CPSR_Z 30
#define CPSR_C 29
#define CPSR_V 28

#define CT_NONE 0
#define CT_DIRECT_MAPPED 1
#define CT_SET_ASSOCIATIVE 2
#define CACHE_MAX_SLOTS 1024


// Dynamic analysis structs

struct analysis_st {
    int i_count;
    int dp_count;
    int mem_count;
    int b_count;
    int b_taken;
    int b_not_taken;
};

// Cache simulation structs

struct cache_slot_st {
    uint32_t valid;
    uint32_t tag;
    uint32_t data;
    uint32_t timestamp;

};
struct cache_st {
    struct cache_slot_st slots[CACHE_MAX_SLOTS];
    int type;
    int size;
    int ways;
    uint32_t index_mask;
    uint32_t index_bits;
    int refs;
    int hits;
    int misses;
    int misses_cold;
    int misses_hot;
    int slots_used;
};

// arm_state represents the state of the emulated ARM processor
struct arm_state {
    uint32_t regs[NREGS];
    uint32_t cpsr;
    uint8_t stack[STACK_SIZE];

    bool analyze;
    bool cache_sim;;
    bool verbose;

    struct analysis_st analysis;
    struct cache_st cache;
};


// Declare the emulation targets
int fib_rec_c(int n);
int fib_rec_s(int n);

int get_bitseq_c(int n, int start, int end);
int get_bitseq_s(int n, int start, int end);

bool is_pal_rec_c(char *s, int start, int end);
bool is_pal_rec_s(char *s, int start, int end);

int max3_c(int a, int b, int c);
int max3_s(int a, int b, int c);

void merge_sort_c(int a[], int aux[], int start, int end);
void merge_sort_s(int a[], int aux[], int start, int end);

int midpoint_c(int start, int end);
int midpoint_s(int start, int end);

int quadratic_c(int x, int a, int b, int c);
int quadratic_s(int x, int a, int b, int c);

void sort_c(int arr[], int len);
void sort_s(int arr[], int len);

void to_upper_c(char *s);
void to_upper_s(char *s);

// Public functions for armemu and cache
void armemu_init(struct arm_state *asp, uint32_t *fp,
                 uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
int armemu(struct arm_state *asp);
void armemu_print(struct arm_state *asp);
void cache_init(struct cache_st *csp);
uint32_t cache_lookup(struct cache_st *csp, uint32_t addr);
void cache_print(struct cache_st *csp);

// Simple logging
void verbose(char *fmt, ...);
