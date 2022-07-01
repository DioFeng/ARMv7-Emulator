#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "armemu.h"

extern bool g_verbose;

void copy_array(int *dst, int *src, int len) {
    for (int i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

void print_array(char *prefix, int *arr, int len) {
    printf("%s", prefix);
    for (int i = 0; i < len; i++) {
        printf(" %d", arr[i]);
    }
    printf("\n");
}

void fib_rec_test(int start_arg, char **argv, struct arm_state *state) {
    int r;
    int n = atoi(argv[start_arg]);

    r = fib_rec_c(n);
    printf("C: %d\n", r);

    r = fib_rec_s(n);
    printf("Asm: %d\n", r);

    armemu_init(state, (uint32_t*) fib_rec_s, n, 0, 0, 0);
    r = armemu(state);
    printf("Emu: %d\n", r);
}

void get_bitseq_test(int start_arg, char **argv, struct arm_state *state) {
    int r;
    int n = atoi(argv[start_arg]);
    int start = atoi(argv[start_arg + 1]);
    int end = atoi(argv[start_arg + 2]);

    r = get_bitseq_c(n, start, end);
    printf("C: %d\n", r);

    r = get_bitseq_s(n, start, end);
    printf("Asm: %d\n", r);

    armemu_init(state, (uint32_t *) get_bitseq_s, n, start, end, 0);
    r = armemu(state);
    printf("Emu: %d\n", r);
}

void is_pal_test(int start_arg, char **argv, struct arm_state *state) {
    bool r;
    char *str = argv[start_arg];
    int end = strlen(str) - 1;

    r = is_pal_rec_c(str, 0, end);
    printf("C: %s\n", r ? "is a palindrome" : "is not a palindrome");

    r = is_pal_rec_s(str, 0, end);
    printf("Asm: %s\n", r ? "is a palindrome" : "is not a palindrome");

    armemu_init(state, (uint32_t*) is_pal_rec_s, (uint32_t) str, 0, end, 0);
    r = armemu(state);
    printf("Emu: %s\n", r ? "is a palindrome" : "is not a palindrome");
}

void max3_test(int start_arg, char **argv, struct arm_state *state) {
    int r;
    int a = atoi(argv[start_arg]);
    int b = atoi(argv[start_arg + 1]);
    int c = atoi(argv[start_arg + 2]);

    r = max3_c(a, b, c);
    printf("C: %d\n", r);

    r = max3_s(a, b, c);
    printf("Asm: %d\n", r);

    armemu_init(state, (uint32_t *) max3_s, a, b, c, 0);
    r = armemu(state);
    printf("Emu: %d\n", r);
}

void merge_sort_test(int start_arg, int argc, char **argv, struct arm_state *state) {
    int aux[MAX_ARRAY];
    int tmp[MAX_ARRAY];
    int a[MAX_ARRAY] = {0,};
    int len = 0;

    for (int i = 0; i < argc - start_arg; i++) {
        a[i] = atoi(argv[i + start_arg]);
        len++;
    }
    
    copy_array(tmp, a, len);
    merge_sort_c(tmp, aux, 0, len - 1);
    print_array("C:", tmp, len);

    copy_array(tmp, a, len);
    merge_sort_s(tmp, aux, 0, len - 1);
    print_array("Asm:", tmp, len);

    copy_array(tmp, a, len);
    armemu_init(state, (uint32_t*) merge_sort_s, (uint32_t) tmp, (uint32_t) aux, 0, len - 1);
    armemu(state);
    print_array("Emu:", tmp, len);
}

void midpoint_test(int start_arg, char **argv, struct arm_state *state) {
    int r;
    int start = atoi(argv[start_arg]);
    int end = atoi(argv[start_arg + 1]);

    r = midpoint_c(start, end);
    printf("C: %d\n", r);

    r = midpoint_s(start, end);
    printf("Asm: %d\n", r);

    armemu_init(state, (uint32_t *) midpoint_s, start, end, 0, 0);
    r = armemu(state);
    printf("Emu: %d\n", r);
}

void quadratic_test(int start_arg, char **argv, struct arm_state *state) {
    int r;
    int x = atoi(argv[start_arg]);
    int a = atoi(argv[start_arg + 1]);
    int b = atoi(argv[start_arg + 2]);
    int c = atoi(argv[start_arg + 3]);

    r = quadratic_c(x, a, b, c);
    printf("C: %d\n", r);

    r = quadratic_s(x, a, b, c);
    printf("Asm: %d\n", r);

    armemu_init(state, (uint32_t *) quadratic_s, x, a, b, c);
    r = armemu(state);
    printf("Emu: %d\n", r);
}

void sort_test(int start_arg, int argc, char **argv, struct arm_state *state) {
    int tmp[MAX_ARRAY];
    int a[MAX_ARRAY] = {0,};
    int len = 0;

    for (int i = 0; i < argc - start_arg; i++) {
        a[i] = atoi(argv[i + start_arg]);
        len++;
    }
    
    copy_array(tmp, a, len);
    sort_c(tmp, len);
    print_array("C:", tmp, len);

    copy_array(tmp, a, len);
    sort_s(tmp, len);
    print_array("Asm:", tmp, len);

    copy_array(tmp, a, len);
    armemu_init(state, (uint32_t*) sort_s, (uint32_t) tmp, len, 0, 0);
    armemu(state);
    print_array("Emu:", tmp, len);
 
}

void to_upper_test(int start_arg, char **argv, struct arm_state *state) {
    // Make a copy of the string since we modify it in place
    char tmp[MAX_ARRAY];
    char *str = argv[start_arg];

    strncpy(tmp, str, MAX_ARRAY);
    to_upper_c(tmp);
    printf("C: %s\n", tmp);

    strncpy(tmp, str, MAX_ARRAY);
    to_upper_s(tmp);
    printf("Asm: %s\n", tmp);

    strncpy(tmp, str, MAX_ARRAY);
    armemu_init(state, (uint32_t*) to_upper_s, (uint32_t) tmp, 0, 0, 0);
    armemu(state);
    printf("Emu: %s\n", tmp);
}

int parse_params(int argc, char **argv, struct arm_state *asp) {
    // Parse optional command line parameters which must precede
    // the name of the emulated program and its parameters
    asp->analyze = false;
    asp->verbose = false;
    asp->cache_sim = false;
    asp->cache.type = CT_NONE;
    int i;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            // Return the index of the first non-optional parameter,
            // which is the name of the emulated program
            break;
        } else if (!strcmp(argv[i], "-a")) {
            asp->analyze = true;
        } else if (!strcmp(argv[i], "-dm")) {
            asp->cache_sim = true;
            asp->cache.type = CT_DIRECT_MAPPED;
            asp->cache.size = atoi(argv[i + 1]);
            i++;
        } else if (!strcmp(argv[i], "-sa")) {
            asp->cache_sim = true;
            asp->cache.type = CT_SET_ASSOCIATIVE;
            asp->cache.size = atoi(argv[i + 1]);
            i++;
        } else if (!strcmp(argv[i], "-v")) {
            g_verbose = true;
        }
    }
    return i;
}

void usage() {
    printf("usage: project04 <prog> [<arg1> ...]\n");
    exit(-1);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        usage();
    }

    struct arm_state state;
    int i = parse_params(argc, argv, &state);
    
    if (!strcmp(argv[i], "fib_rec")) {
        fib_rec_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "get_bitseq")) {
        get_bitseq_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "is_pal")) {
        is_pal_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "max3")) {
        max3_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "merge_sort")) {
        merge_sort_test(i + 1, argc, argv, &state);
    } else if (!strcmp(argv[i], "midpoint")) {
        midpoint_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "quadratic")) {
        quadratic_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "sort")) {
        sort_test(i + 1, argc, argv, &state);
    } else if (!strcmp(argv[i], "to_upper")) {
        to_upper_test(i + 1, argv, &state);
    } else {
        usage();
    }

    armemu_print(&state);

    return 0;
}
