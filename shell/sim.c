#include <stdint.h>
#include <stdlib.h>

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


/* Global state variables */
struct CPUState cpu_state;
struct MemoryRegion mem_region[NB_REGIONS] = {
	{MEM_TEXT_START, MEM_TEXT_SIZE, NULL},
	{MEM_DATA_START, MEM_DATA_SIZE, NULL},
};

/** Allocate memory for memory. */
static void mem_init()
{
	int i;
	for (i = 0; i < NB_REGIONS; i++) {
		mem_region[i].mem = malloc(sizeof(uint8_t) * mem_region[i].size);
	}
}

/** Reset PC and set other registers to 0 */
static void reset_cpu()
{
	int i;
	cpu_state.CPSR = 0x00;
	for (i = 0; i < NB_REGS; i++) {
		cpu_state.regs[i] = 0x00;	
	}
}
