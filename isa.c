#include "isa_helper.h"
#include "isa.h"

/* Instructions to implement:
 * ADC ADD AND B BIC
 * BL CMN CMP EOR LDR
 * LDRB MLA MOV MUL MVN
 * ORR RSB RSC SBC STR
 * STRB SUB TEQ TST SWI
 */

static void decode_and_exec(uint32_t instruction);
static void exec_LDR(uint32_t instruction);
static void exec_STR(uint32_t instruction);
static void exec_STRB(uint32_t instruction);

static struct CPUState next_state, curr_state;

struct CPUState process_instruction(struct CPUState state)
{
    next_state = curr_state = state;
    uint32_t instruction = mem_read_32(curr_state.regs[PC]);
    decode_and_exec(instruction);
    next_state.regs[PC] += 4;
    return next_state;
}

static void decode_and_exec(uint32_t instruction)
{
    if (get_bits(instruction, 27, 25) == 0x2 ||
        (get_bits(instruction, 27, 25) ==  0x3 && get_bit(instruction, 4) == 0)) {
        // LOAD STORE INSTRUCTIONS
        if (get_bit(instruction, B_BIT) == 0) {
            if (get_bit(instruction, L_BIT) == 1) { // LDR
                exec_LDR(instruction);
            } else { // STR
                exec_STR(instruction);
            }
        } else {
            if (get_bit(instruction, L_BIT) == 1) { // LDRB
                // @RadhikaG has to do this
            } else { // STRB
                exec_STRB(instruction);
            }
        }
    }
}

static void exec_LDR(uint32_t instruction)
{
    if (condition_check(curr_state, get_bits(instruction, 31, 28))) {
        uint32_t address = ld_str_addr_mode(curr_state, &next_state, instruction);
        uint32_t data = mem_read_32(address);
        uint32_t rd_id = get_bits(instruction, 15, 12);
        next_state.regs[rd_id] = data;
    }
}

static void exec_STR(uint32_t instruction)
{
    if (condition_check(curr_state, get_bits(instruction, 31, 28))) {
        uint32_t rd_id = get_bits(instruction, 15, 12);
        uint32_t data = curr_state.regs[rd_id];
        uint32_t address = ld_str_addr_mode(curr_state, &next_state, instruction);
        mem_write_32(address, data);
    }
}

static void exec_STRB(uint32_t instruction)
{
    if (condition_check(curr_state, get_bits(instruction, 31, 28))) {
        uint32_t rd_id = get_bits(instruction, 15, 12);
        uint8_t data = curr_state.regs[rd_id] & 0xff; // LSB byte of reg
        uint32_t address = ld_str_addr_mode(curr_state, &next_state, instruction);
        mem_write_8(address, data);
    }
}

