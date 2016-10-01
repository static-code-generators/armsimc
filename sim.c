#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sim.h"
#include "isa.h"

static struct CPUState cpu_state;

static struct MemoryRegion mem_region[NB_REGIONS] = {
    {MEM_TEXT_START, MEM_TEXT_SIZE, NULL},
    {MEM_DATA_START, MEM_DATA_SIZE, NULL},
};

void initialize()
{
    int i;
    reset_cpu();
    for (i = 0; i < NB_REGIONS; i++) {
        mem_region[i].mem = malloc(sizeof(uint8_t) * mem_region[i].size);
        memset(mem_region[i].mem, 0, sizeof(uint8_t) * mem_region[i].size);
    }
}

void reset_cpu()
{
    int i;
    cpu_state.CPSR = 0x00;
    for (i = 0; i < NB_REGS; i++) {
        cpu_state.regs[i] = 0x00;
    }
    cpu_state.regs[PC] = mem_region[MEM_TEXT].start;
    cpu_state.halted = 0;
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

void mem_write_32(uint32_t address, uint32_t data)
{
    struct MemoryRegion *region = find_mem_region(address);
    assert(region != NULL);
    uint32_t offset = address - region->start;
    region->mem[offset+0] = (data >> 24) & 0xFF;
    region->mem[offset+1] = (data >> 16) & 0xFF;
    region->mem[offset+2] = (data >>  8) & 0xFF;
    region->mem[offset+3] = (data >>  0) & 0xFF;
}

uint32_t mem_read_32(uint32_t address)
{
    struct MemoryRegion *region = find_mem_region(address);
    assert(region != NULL);
    uint32_t offset = address - region->start;
    return
        (region->mem[offset+0] << 24) |
        (region->mem[offset+1] << 16) |
        (region->mem[offset+2] <<  8) |
        (region->mem[offset+3] <<  0);
}

void load_program(FILE *fp)
{
    uint32_t instruction;
    uint32_t addr = mem_region[MEM_TEXT].start;
    while (fscanf(fp, "%x\n", &instruction) != EOF) {
        mem_write_32(addr, instruction);
        addr += 4;
    }
}

int cpu_cycle()
{
    if (!cpu_state.halted) {
        cpu_state = process_instruction(cpu_state);
    }
    return -cpu_state.halted;
}

struct CPUState get_cpu_state()
{
    return cpu_state;
}
