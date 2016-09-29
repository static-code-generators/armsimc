#ifndef SIM_H
#define SIM_H

#define NB_REGS 16

struct CPUState {
    uint32_t regs[NB_REGS]; ///> Register File
    uint32_t CPSR; ///> Current Program Status Register
};

// We are not implementing stack ops right now
// The values below have been chosen arbitrarily
#define MEM_TEXT_START 0x00000000
#define MEM_TEXT_SIZE  0x00100000
#define MEM_DATA_START 0x20000000
#define MEM_DATA_SIZE  0x00100000

#define NB_REGIONS 2
#define MEM_TEXT 0
#define MEM_DATA 1

struct MemoryRegion {
    uint32_t start;
    uint32_t size;
    uint8_t *mem;
};

void initialize();
void load_program(FILE *code);
void reset_cpu();

#endif
