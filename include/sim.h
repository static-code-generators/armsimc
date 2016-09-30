#ifndef SIM_H
#define SIM_H

#include <stdint.h>
#include <stdio.h>

#define NB_REGS 16

#define PC 15

struct CPUState {
    uint32_t regs[NB_REGS]; ///> Register File
    uint32_t CPSR; ///> Current Program Status Register
    uint8_t halted; ///> 1 if halted, 0 if running
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

/** Allocate memory, initialize CPU states */
void initialize();
/** Set all registers to 0 */
void reset_cpu();
/** Load program into memory */
void load_program(FILE *code);
/** Write 32-bit data to address (Big-Endian) */
void mem_write_32(uint32_t address, uint32_t data);
/** Read 32-bit data from address (Big-Endian) */
uint32_t mem_read_32(uint32_t address);
/** Execute CPU cycle.
 * \return 0 for success, -1 for halted
 */
int cpu_cycle();

#endif
