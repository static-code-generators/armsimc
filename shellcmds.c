#include "shellcmds.h"
#include "sim.h"
#include <stdio.h>
#include <stdint.h>

static int initialized = 0;
#define CHECK_INIT if (!initialized) { printf("No program loaded\n"); return; }

void cmd_run()
{
    CHECK_INIT;
    int cnt = 0;
    while (cpu_cycle() >= 0) {
        cnt++;
    }
    printf("CPU Halted at %dth instruction\n", cnt);
}

void cmd_file(char *fname)
{
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", fname);
        return;
    }
    initialize();
    load_program(fp);
    initialized = 1;
    fclose(fp);
}

void cmd_step(int nbstep) {
    CHECK_INIT;
    for (int i = 0; i < nbstep; i++) {
        if (cpu_cycle() < 0) {
            printf("CPU Halted @ %dth step\n", i+1);
            return;
        }
    }
    printf("Successfully executed %d instructions\n", nbstep);
}

void cmd_mdump(uint32_t low_addr, uint32_t high_addr, char *fname)
{
    CHECK_INIT;
    FILE *fp;
    if (fname == NULL) {
        fp = stdout;
    } else {
        fp = fopen(fname, "w");
        if (fp == NULL) {
            fprintf(stderr, "Error: Could not open file %s\n", fname);
            return;
        }
    }
    uint32_t word;
    for (uint32_t addr = low_addr; addr <= high_addr; addr += 4) {
        word = mem_read_32(addr);
        fprintf(fp, "%08x: %08x\n", addr, word);
    }
    if (fp != stdout) {
        fclose(fp);
    }
}

void cmd_rdump(char *fname)
{
    CHECK_INIT;
    FILE *fp;
    if (fname == NULL) {
        fp = stdout;
    } else {
        fp = fopen(fname, "w");
        if (fp == NULL) {
            fprintf(stderr, "Error: Could not open file %s\n", fname);
            return;
        }
    }
    struct CPUState state = get_cpu_state();
    fprintf(fp, "HALTED: %s\n", state.halted ? "Yes" : "No");
    for (int i = 0; i <= 14; i++) {
        fprintf(fp, "   r%02d: %08x\n", i, state.regs[i]);
    }
    fprintf(fp, "    PC: %08x\n", state.regs[15]);
    fprintf(fp, "  CPSR: %08x\n", state.CPSR);
    fprintf(fp, "  (N: %d, Z: %d, C: %d, V: %d)\n",
                 (state.CPSR >> CPSR_N) & 1,
                 (state.CPSR >> CPSR_Z) & 1,
                 (state.CPSR >> CPSR_C) & 1,
                 (state.CPSR >> CPSR_V) & 1);
    if (fp != stdout) {
        fclose(fp);
    }
}

void cmd_set(int reg_num, uint32_t reg_val)
{
    CHECK_INIT;
    set_reg(reg_num, reg_val);
}

void cmd_help()
{
    printf("`r` or `run`: simulate the program until it indicates that the simulator should halt.\n");
    printf("`file <hexfile>`: load this file in program memory.\n");
    printf("`step [i]`: execute one instruction (or optionally `i`)\n");
    printf("`mdump 0x<low> 0x<high> [dumpfile]`: dump the contents of memory, from location low to location high to the screen or to the dump file [dumpfile].\n");
    printf("`rdump [dumpfile]`: dump the current instruction count, the contents of R0 – R14, R15 (PC), and the CPSR to the screen or to the file [dumpfile].\n");
    printf("`set r<n> 0x<reg_val>`: set general purpose register reg r_n to value reg_val.\n");
    printf("`?` or `help`: print out a list of all shell commands.\n");
    printf("`q` or `quit`: quit the shell.\n");
}

