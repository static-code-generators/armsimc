#include "isa_helper.h"
#include "isa.h"

/* Instructions to implement:
 * ADC ADD AND B BIC
 * BL CMN CMP EOR LDR
 * LDRB MLA MOV MUL MVN
 * ORR RSB RSC SBC STR
 * STRB SUB TEQ TST SWI
 */

enum DataProcOpcode {
    OP_AND, OP_EOR, OP_SUB, OP_RSB,
    OP_ADD, OP_ADC, OP_SBC, OP_RSC,
    OP_TST, OP_TEQ, OP_CMP, OP_CMN,
    OP_ORR, OP_MOV, OP_BIC, OP_MVN,
};

static void decode_and_exec(uint32_t instruction);
static void exec_LDR(uint32_t instruction);
static void exec_STR(uint32_t instruction);
static void exec_STRB(uint32_t instruction);
static void exec_SWI(uint32_t instruction);
static void exec_RSC(uint32_t instruction);

static struct CPUState next_state, curr_state;

struct CPUState process_instruction(struct CPUState state)
{
    if (state.halted) {
        return state;
    }
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
    } else if (get_bits(instruction, 27, 26) ==  0x0) {
        // DATA PROCESSING INSTRUCTIONS
        uint8_t opcode = get_bits(instruction, 24, 21);
        switch (opcode) {
            case OP_RSC: exec_RSC(instruction); break;
        }
    } else if (get_bits(instruction, 27, 24) == 0xf) { // SWI
        exec_SWI(instruction);
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

// reverse subtract, with carry
static void exec_RSC(uint32_t instruction)
{
    if (condition_check(curr_state, get_bits(instruction, 31, 28))) {
        struct ShifterOperand * shiftop;
        shiftop = shifter_operand(curr_state, get_bits(instruction, 11, 0));
        uint8_t rd_id = get_bits(instruction, 15, 12);
        uint8_t rn_id = get_bits(instruction, 19, 16);
        uint32_t rn_val = curr_state.regs[rn_id];
        bool carry = get_bit(curr_state.CPSR, CPSR_C);
        uint32_t rd_val = shiftop->shifter_operand - rn_val - !carry;
        next_state.regs[rd_id] = rd_val;
        if (get_bit(instruction, S_BIT) == 1) {
            set_bit(&next_state.CPSR, CPSR_N, get_bit(rd_val, 31));
            set_bit(&next_state.CPSR, CPSR_Z, (rd_val ? 0 : 1));
            set_bit(&next_state.CPSR, CPSR_C,
                    !check_sub_borrow(rd_val, rn_val + !carry)); // c = !b
            set_bit(&next_state.CPSR, CPSR_V,
                    check_overflow(rd_val, -(rn_val + !carry)));
        }
    }
}

/* So we don't do the normal SWI stuff as we have no OS, we just check if we got
 * `swi #10` and if yes, we halt processor and bye bye
 */
static void exec_SWI(uint32_t instruction)
{
    if (condition_check(curr_state, get_bits(instruction, 31, 28))) {
        uint32_t immed_24 = get_bits(instruction, 23, 0);
        if (immed_24 == 10) {
            next_state.halted = 1;
        }
    }
}
