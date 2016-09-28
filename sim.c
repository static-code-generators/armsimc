#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sim.h"

/* Global state variables */
struct CPUState state_current, state_next;
struct MemoryRegion mem_region[NB_REGIONS] = {
    {MEM_TEXT_START, MEM_TEXT_SIZE, NULL},
    {MEM_DATA_START, MEM_DATA_SIZE, NULL},
};

/** Allocate memory, initialze CPU states */
void initialize()
{
    int i;
    reset_cpu();
    for (i = 0; i < NB_REGIONS; i++) {
        mem_region[i].mem = malloc(sizeof(uint8_t) * mem_region[i].size);
        memset(mem_region[i].mem, 0, sizeof(uint8_t) * mem_region[i].size);
    }
    state_next = state_current;
}

/** Set all registers to 0 */
void reset_cpu()
{
    int i;
    state_current.CPSR = 0x00;
    for (i = 0; i < NB_REGS; i++) {
        state_current.regs[i] = 0x00;
    }
}

/** Find memory region of an address */
static struct MemoryRegion * find_mem_region(uint32_t address) {
    for (int i = 0; i < NB_REGIONS; i++) {
        if (mem_region[i].start <= address &&
            address < (mem_region[i].start + mem_region[i].size)) {
            return &mem_region[i];
        }
    }
    return NULL;
}

/** Write 32-bit data to address (Big-Endian) */
static void mem_write_32(uint32_t address, uint32_t data)
{
    struct MemoryRegion *region = find_mem_region(address);
    assert(region != NULL);
    uint32_t offset = address - region->start;
    *((uint32_t *) (region->mem + offset)) = data;
}

/** Read 32-bit data from address (Big-Endian) */
static uint32_t mem_read_32(uint32_t address)
{
    struct MemoryRegion *region = find_mem_region(address);
    assert(region != NULL);
    uint32_t offset = address - region->start;
    uint32_t data = *((uint32_t *) (region->mem + offset));
    return data;
}

/** Load program into memory */
void load_program(FILE *code)
{
    uint32_t word;
    int i = 0;
    while (fscanf(code, "%x\n", &word) != EOF) {
        mem_write_32(MEM_TEXT_START + i, word);
        i += 4;
    }
}
