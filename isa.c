#include "isa_helper.h"
#include "isa.h"

/* Instructions to implement:
 * ADC ADD AND B BIC
 * BL CMN CMP EOR LDR
 * LDRB MLA MOV MUL MVN
 * ORR RSB RSC SBC STR STRB SUB TEQ TST SWI */

enum DataProcOpcode {
    OP_AND, OP_EOR, OP_SUB, OP_RSB,
    OP_ADD, OP_ADC, OP_SBC, OP_RSC,
    OP_TST, OP_TEQ, OP_CMP, OP_CMN,
    OP_ORR, OP_MOV, OP_BIC, OP_MVN,
};

static void decode_and_exec(uint32_t instruction);
static void exec_ADC(uint32_t instruction);
static void exec_ADD(uint32_t instruction);
static void exec_AND(uint32_t instruction);
static void exec_B(uint32_t instruction);
static void exec_BIC(uint32_t instruction);
static void exec_LDR(uint32_t instruction);
static void exec_STR(uint32_t instruction);
static void exec_STRB(uint32_t instruction);
static void exec_SWI(uint32_t instruction);
static void exec_RSC(uint32_t instruction);
static void exec_CMP(uint32_t instruction);
static void exec_CMN(uint32_t instruction);
static void exec_EOR(uint32_t instruction);
static void exec_ORR(uint32_t instruction);
static void exec_TST(uint32_t instruction);
static void exec_BL(uint32_t instruction);
static void exec_RSB(uint32_t instruction);
static void exec_SUB(uint32_t instruction);
static void exec_TEQ(uint32_t instruction);
static void exec_SBC(uint32_t instruction);
static void exec_LDRB(uint32_t instruction);
static void exec_MUL(uint32_t instruction);
static void exec_MLA(uint32_t instruction);
static void exec_MOV(uint32_t instruction);
static void exec_MVN(uint32_t instruction);

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
    if (!condition_check(curr_state, get_bits(instruction, 31, 28))) {
        return;
    }
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
                exec_LDRB(instruction);
                // no ure mum does dis evry night
            } else { // STRB
                exec_STRB(instruction);
            }
        }
    } else if ((get_bits(instruction, 27, 25) == 0x0 &&
                    (!get_bit(instruction, 4) ||
                         (!get_bit(instruction, 7) && get_bit(instruction, 4)))) ||
               (get_bits(instruction, 27, 25) == 0x1)) {
        // DATA PROCESSING INSTRUCTIONS
        enum DataProcOpcode opcode = get_bits(instruction, 24, 21);
        switch (opcode) {
            case OP_RSC: exec_RSC(instruction); break;
            case OP_CMP: exec_CMP(instruction); break;
            case OP_CMN: exec_CMN(instruction); break;
            case OP_EOR: exec_EOR(instruction); break;
            case OP_ORR: exec_ORR(instruction); break;
            case OP_TST: exec_TST(instruction); break;
            case OP_MOV: exec_MOV(instruction); break;
            case OP_MVN: exec_MVN(instruction); break;
        }
    } else if (get_bits(instruction, 27, 24) == 0xf) { // SWI
        exec_SWI(instruction);
    } else if (get_bits(instruction, 27, 24) == 0x0 && get_bits(instruction, 7, 4) == 0x9) {
        // MULTIPLY INSTRUCTIONS
        if (get_bits(instruction, 23, 21) == 0x1) { // MLA
            exec_MLA(instruction);
        } else if (get_bits(instruction, 23, 21) == 0x0) { // MUL
            exec_MUL(instruction);
        }
    }
}

static void exec_LDR(uint32_t instruction)
{
    uint32_t address = ld_str_addr_mode(curr_state, &next_state, instruction);
    uint32_t data = mem_read_32(address);
    uint32_t rd_id = get_bits(instruction, 15, 12);
    next_state.regs[rd_id] = data;
}

static void exec_STR(uint32_t instruction)
{
    uint32_t rd_id = get_bits(instruction, 15, 12);
    uint32_t data = curr_state.regs[rd_id];
    uint32_t address = ld_str_addr_mode(curr_state, &next_state, instruction);
    mem_write_32(address, data);
}

static void exec_STRB(uint32_t instruction)
{
    uint32_t rd_id = get_bits(instruction, 15, 12);
    uint8_t data = curr_state.regs[rd_id] & 0xff; // LSB byte of reg
    uint32_t address = ld_str_addr_mode(curr_state, &next_state, instruction);
    mem_write_8(address, data);
}

static void exec_CMN(uint32_t instruction)
{
    uint32_t Rn_addr = get_bits(instruction, 19, 16);
    struct ShifterOperand *shifter_op = shifter_operand(curr_state, instruction);
    uint32_t op1 = curr_state.regs[Rn_addr];
    uint32_t op2 = shifter_op->shifter_operand;

    uint32_t alu_out = op1 + op2;

    set_bit(&(next_state.CPSR), CPSR_N, get_bit(alu_out, 31));
    set_bit(&(next_state.CPSR), CPSR_Z, alu_out ? 0 : 1);
    set_bit(&(next_state.CPSR), CPSR_C, check_add_carry(op1, op2));
    set_bit(&(next_state.CPSR), CPSR_V, check_overflow(op1, op2));
}

static void exec_CMP(uint32_t instruction)
{
    uint32_t Rn_addr = get_bits(instruction, 19, 16);
    struct ShifterOperand *shifter_op = shifter_operand(curr_state, instruction);
    uint32_t op1 = curr_state.regs[Rn_addr];
    uint32_t op2 = shifter_op->shifter_operand;

    uint32_t alu_out = op1 - op2;

    set_bit(&(next_state.CPSR), CPSR_N, get_bit(alu_out, 31));
    set_bit(&(next_state.CPSR), CPSR_Z, alu_out ? 0 : 1);
    set_bit(&(next_state.CPSR), CPSR_C, !check_sub_borrow(op1, op2));
    set_bit(&(next_state.CPSR), CPSR_V, check_overflow(op1, -op2));
}

static void exec_EOR(uint32_t instruction)
{
    uint32_t Rn_addr = get_bits(instruction, 19, 16);
    uint32_t Rd_addr = get_bits(instruction, 15, 12);
    struct ShifterOperand *shifter_op = shifter_operand(curr_state, instruction);

    next_state.regs[Rd_addr] = curr_state.regs[Rn_addr] ^ shifter_op->shifter_operand;

    if(get_bit(instruction, S_BIT) == 1){
        set_bit(&(next_state.CPSR), CPSR_N, get_bit(curr_state.regs[Rd_addr], 31));
        set_bit(&(next_state.CPSR), CPSR_Z, next_state.regs[Rd_addr] ? 0 : 1);
        set_bit(&(next_state.CPSR), CPSR_C, shifter_op->shifter_carry);
    }
}

// reverse subtract, with carry
static void exec_RSC(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
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
                !check_sub_borrow(shiftop->shifter_operand, rn_val + !carry)); // c = !b
        set_bit(&next_state.CPSR, CPSR_V,
                check_overflow(shiftop->shifter_operand, -(rn_val + !carry)));
    }
}

static void exec_ORR(uint32_t instruction)
{
    uint32_t Rn_addr = get_bits(instruction, 19, 16);
    uint32_t Rd_addr = get_bits(instruction, 15, 12);
    struct ShifterOperand *shifter_op = shifter_operand(curr_state, instruction);

    next_state.regs[Rd_addr] = curr_state.regs[Rn_addr] | shifter_op->shifter_operand;

    if(get_bit(instruction, S_BIT) == 1){
        set_bit(&(next_state.CPSR), CPSR_N, get_bit(curr_state.regs[Rd_addr], 31));
        set_bit(&(next_state.CPSR), CPSR_Z, next_state.regs[Rd_addr] ? 0 : 1);
        set_bit(&(next_state.CPSR), CPSR_C, shifter_op->shifter_carry);
    }
}

static void exec_TST(uint32_t instruction)
{
    uint32_t Rn_addr = get_bits(instruction, 19, 16);
    struct ShifterOperand *shifter_op = shifter_operand(curr_state, instruction);

    uint32_t alu_out = curr_state.regs[Rn_addr] & shifter_op->shifter_operand;

    set_bit(&(next_state.CPSR), CPSR_N, get_bit(alu_out, 31));
    set_bit(&(next_state.CPSR), CPSR_Z, alu_out ? 0 : 1);
    set_bit(&(next_state.CPSR), CPSR_C, shifter_op->shifter_carry);

}

/* So we don't do the normal SWI stuff as we have no OS, we just check if we got
 * `swi #10` and if yes, we halt processor and bye bye
 */
static void exec_SWI(uint32_t instruction)
{
    uint32_t immed_24 = get_bits(instruction, 23, 0);
    if (immed_24 == 10) {
        next_state.halted = 1;
    }
}

static void exec_ADC(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint8_t rd_id = get_bits(instruction, 15, 12);
    uint8_t rn_id = get_bits(instruction, 19, 16);
    uint32_t rn_val = curr_state.regs[rn_id];
    bool carry = get_bit(curr_state.CPSR, CPSR_C);
    uint32_t rd_val = rn_val + shiftop->shifter_operand + carry;
    next_state.regs[rd_id] = rd_val;
    if (get_bit(instruction, S_BIT) == 1) {
        set_bit(&next_state.CPSR, CPSR_N, get_bit(rd_val, 31));
        set_bit(&next_state.CPSR, CPSR_Z, (rd_val ? 0 : 1));
        set_bit(&next_state.CPSR, CPSR_C,
                check_add_carry(rn_val, shiftop->shifter_operand + carry));
        set_bit(&next_state.CPSR, CPSR_V,
                check_overflow(rn_val, shiftop->shifter_operand + carry));
    }
}

static void exec_ADD(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint8_t rd_id = get_bits(instruction, 15, 12);
    uint8_t rn_id = get_bits(instruction, 19, 16);
    uint32_t rn_val = curr_state.regs[rn_id];
    uint32_t rd_val = rn_val + shiftop->shifter_operand;
    next_state.regs[rd_id] = rd_val;
    if (get_bit(instruction, S_BIT) == 1) {
        set_bit(&next_state.CPSR, CPSR_N, get_bit(rd_val, 31));
        set_bit(&next_state.CPSR, CPSR_Z, (rd_val ? 0 : 1));
        set_bit(&next_state.CPSR, CPSR_C,
                check_add_carry(rn_val, shiftop->shifter_operand));
        set_bit(&next_state.CPSR, CPSR_V,
                check_overflow(rn_val, shiftop->shifter_operand));
    }
}

static void exec_AND(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint8_t rd_id = get_bits(instruction, 15, 12);
    uint8_t rn_id = get_bits(instruction, 19, 16);
    uint32_t rn_val = curr_state.regs[rn_id];
    uint32_t rd_val = rn_val & shiftop->shifter_operand;
    next_state.regs[rd_id] = rd_val;
    if (get_bit(instruction, S_BIT) == 1) {
        set_bit(&next_state.CPSR, CPSR_N, get_bit(rd_val, 31));
        set_bit(&next_state.CPSR, CPSR_Z, (rd_val ? 0 : 1));
        set_bit(&next_state.CPSR, CPSR_C,shiftop->shifter_carry);
        //flag V unaffected
    }
}

static void exec_BIC(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint8_t rd_id = get_bits(instruction, 15, 12);
    uint8_t rn_id = get_bits(instruction, 19, 16);
    uint32_t rn_val = curr_state.regs[rn_id];
    uint32_t rd_val = rn_val & !(shiftop->shifter_operand);
    next_state.regs[rd_id] = rd_val;
    if (get_bit(instruction, S_BIT) == 1) {
        set_bit(&next_state.CPSR, CPSR_N, get_bit(rd_val, 31));
        set_bit(&next_state.CPSR, CPSR_Z, (rd_val ? 0 : 1));
        set_bit(&next_state.CPSR, CPSR_C,shiftop->shifter_carry);
        //flag V unaffected
    }
}

static void exec_BL(uint32_t instruction)
{
    uint32_t immed_24 = get_bits(instruction, 23, 0);
    uint8_t L = get_bit(instruction, 24);
    if (L == 1) {
        next_state.regs[LR] = curr_state.regs[PC] + 4;
    }
    next_state.regs[PC] += (int32_t)(sign_extend(immed_24, 24, 30) << 2);
}

static void exec_RSB(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint8_t Rdi = get_bits(instruction, 15, 12);
    uint8_t Rni = get_bits(instruction, 19, 16);
    uint8_t S = get_bit(instruction, 20);
    next_state.regs[Rdi] = shiftop->shifter_operand - curr_state.regs[Rni];
    if (S == 1) { //ignore the SPSR crap.
        set_bit(&next_state.CPSR, CPSR_N, get_bit(curr_state.regs[Rdi], 31));
        set_bit(&next_state.CPSR, CPSR_Z, !curr_state.regs[Rdi]);
        set_bit(&(next_state.CPSR), CPSR_C, !check_sub_borrow(shiftop->shifter_operand, curr_state.regs[Rni]));
        set_bit(&(next_state.CPSR), CPSR_V, check_overflow(shiftop->shifter_operand, -curr_state.regs[Rni]));
    }
}

static void exec_SUB(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint8_t Rdi = get_bits(instruction, 15, 12);
    uint8_t Rni = get_bits(instruction, 19, 16);
    uint8_t S = get_bit(instruction, 20);
    next_state.regs[Rdi] = curr_state.regs[Rni] - shiftop->shifter_operand;
    if (S == 1) { //ignore the SPSR crap.
        set_bit(&next_state.CPSR, CPSR_N, get_bit(curr_state.regs[Rdi], 31));
        set_bit(&next_state.CPSR, CPSR_Z, !curr_state.regs[Rdi]);
        set_bit(&(next_state.CPSR), CPSR_C, !check_sub_borrow(curr_state.regs[Rni], shiftop->shifter_operand));
        set_bit(&(next_state.CPSR), CPSR_V, check_overflow(curr_state.regs[Rni], -shiftop->shifter_operand));
    }
}

static void exec_TEQ(uint32_t instruction)
{

    uint32_t Rn_addr = get_bits(instruction, 19, 16);
    struct ShifterOperand *shifter_op = shifter_operand(curr_state, instruction);
    uint32_t op1 = curr_state.regs[Rn_addr];
    uint32_t op2 = shifter_op->shifter_operand;

    uint32_t alu_out = op1 ^ op2;

    set_bit(&(next_state.CPSR), CPSR_N, get_bit(alu_out, 31));
    set_bit(&(next_state.CPSR), CPSR_Z, !alu_out);
    set_bit(&(next_state.CPSR), CPSR_C, shifter_op->shifter_carry);
    //VFlag unaffected.
}

static void exec_SBC(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint8_t Rdi = get_bits(instruction, 15, 12);
    uint8_t Rni = get_bits(instruction, 19, 16);
    uint8_t S = get_bit(instruction, 20);
    next_state.regs[Rdi] = curr_state.regs[Rni] - shiftop->shifter_operand - !get_bit(curr_state.CPSR, CPSR_C);
    if (S == 1) { //ignore the SPSR crap.
        set_bit(&next_state.CPSR, CPSR_N, get_bit(curr_state.regs[Rdi], 31));
        set_bit(&next_state.CPSR, CPSR_Z, !curr_state.regs[Rdi]);
        set_bit(&(next_state.CPSR), CPSR_C, !check_sub_borrow(curr_state.regs[Rni], shiftop->shifter_operand + !get_bit(curr_state.CPSR, CPSR_C)));
        set_bit(&(next_state.CPSR), CPSR_V, check_overflow(curr_state.regs[Rni], -(shiftop->shifter_operand + !get_bit(curr_state.CPSR, CPSR_C))));
    }
}

static void exec_LDRB(uint32_t instruction)
{
    uint8_t data = mem_read_8(ld_str_addr_mode(curr_state, &(next_state), instruction));
    uint32_t rd_id = get_bits(instruction, 15, 12);
    next_state.regs[rd_id] = data; // casting uint8_t to uint32_t zeros top 3 bytes on its own; done to store byte to LSB of rd_id
}

// Multiply and Multiply-Accumulate Instructions
// MUL and MLA produce the low 32 bits of a multiply.
// TIL: The results of a signed multiply and an unsigned multiply
// differ only in the upper 32 bits; the low 32 bits remain the same for both.
// So we can use the MUL and MLA ops unchanged for signed and unsigned operands.
static void exec_MUL(uint32_t instruction)
{
    uint32_t rd_id = get_bits(instruction, 19, 16);
    uint32_t rs_id = get_bits(instruction, 11, 8);
    uint32_t rm_id = get_bits(instruction, 3, 0);

    uint32_t result = curr_state.regs[rm_id] * curr_state.regs[rs_id]; // we don't care about overflows; no need to set C(arry) flag
    next_state.regs[rd_id] = result;
    
    if (get_bit(instruction, S_BIT)) {
        set_bit(&(next_state.CPSR), CPSR_N, get_bit(result, 31));
        set_bit(&(next_state.CPSR), CPSR_Z, ((result) ? 0 : 1));
    }
}

static void exec_MLA(uint32_t instruction)
{
    uint32_t rd_id = get_bits(instruction, 19, 16);
    uint32_t rn_id = get_bits(instruction, 15, 12);
    uint32_t rs_id = get_bits(instruction, 11, 8);
    uint32_t rm_id = get_bits(instruction, 3, 0);

    uint32_t result = (curr_state.regs[rm_id] * curr_state.regs[rs_id]) + curr_state.regs[rn_id]; // we don't care about overflows; no need to set C(arry) flag
    next_state.regs[rd_id] = result;
    
    if (get_bit(instruction, S_BIT)) {
        set_bit(&(next_state.CPSR), CPSR_N, get_bit(result, 31));
        set_bit(&(next_state.CPSR), CPSR_Z, ((result) ? 0 : 1));
    }
}

static void exec_MOV(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint32_t val = shiftop->shifter_operand;
    uint32_t rd_id = get_bits(instruction, 15, 12);
    curr_state.regs[rd_id] = val;

    if (get_bit(instruction, S_BIT)) {
        set_bit(&(next_state.CPSR), CPSR_N, get_bit(val, 31));
        set_bit(&(next_state.CPSR), CPSR_Z, ((val) ? 0 : 1));
        set_bit(&(next_state.CPSR), CPSR_C, shiftop->shifter_carry);
    }
}

static void exec_MVN(uint32_t instruction)
{
    struct ShifterOperand * shiftop;
    shiftop = shifter_operand(curr_state, instruction);
    uint32_t val = ~(shiftop->shifter_operand) & 0xFFFFFFFF; // bitwise negation promotes result to (int); bitwise and-ing done to prevent this
    uint32_t rd_id = get_bits(instruction, 15, 12);
    curr_state.regs[rd_id] = val;

    if (get_bit(instruction, S_BIT)) {
        set_bit(&(next_state.CPSR), CPSR_N, get_bit(val, 31));
        set_bit(&(next_state.CPSR), CPSR_Z, ((val) ? 0 : 1));
        set_bit(&(next_state.CPSR), CPSR_C, shiftop->shifter_carry);
    }
}
