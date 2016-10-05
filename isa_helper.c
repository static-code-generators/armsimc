#include "sim.h"
#include "isa_helper.h"

uint32_t rotate_right(uint32_t shiftee, uint8_t shifter)
{
    return (shiftee << (32 - shifter)) | (shiftee >> (shifter));
}

uint8_t get_bit(uint32_t from, uint8_t bitid)
{
    return (from >> bitid) & 1;
}

uint32_t get_bits(uint32_t from, uint8_t msb_id, uint8_t lsb_id)
{
    uint8_t nb_bits = msb_id - lsb_id + 1;
    uint32_t mask = (1 << nb_bits) - 1;
    return (from >> lsb_id) & mask;
}

bool check_add_carry(uint32_t a, uint32_t b)
{
    uint32_t res = a + b;
    return (res < a);
}

bool check_sub_borrow(uint32_t a, uint32_t b)
{
    uint32_t res = a - b;
    return (res > a);
}

bool check_overflow(uint32_t a, uint32_t b)
{
    uint32_t res = a + b;
    // overflow = !a_31.!b_31.res_31 || a_31.b_31.!res_31
    return (!(a >> 31) && !(b >> 31) && (res >> 31)) ||
           ((a >> 31) && (b >> 31) && !(res >> 31));
}

void set_bit(uint32_t *reg, uint8_t bit_id, uint8_t bit_val)
{
    if(bit_val == 1){
        *(reg) |= (1 << bit_id);
    }
    else{
        *(reg) &= ~(1 << bit_id);
    }
}

struct ShifterOperand * shifter_operand(struct CPUState state, uint32_t instruction)
{
    struct ShifterOperand *retval = malloc(sizeof(struct ShifterOperand));
    enum ShifterType {
        LSLIMM = 0, //Logical shift left by immediate
        LSLREG = 1, //Logical shift left by reg.
        LSRIMM = 2, //Logical shift right by immediate
        LSRREG = 3,
        ASRIMM = 4, //Arithmetic shift right by immediate
        ASRREG = 5, 
        RORIMM = 6, //Rotate right by immediate
        RORREG = 7
    } s_type;
    if (instruction & (1 << I_BIT)) { //immediate
        uint8_t rotate_imm = (instruction >> 8) & 0xF; //bits 11-8.
        rotate_imm <<= 1; //actually have to rotate by 2x
        uint8_t immed_8 = instruction & 0xFF; //bits 7-0.
        retval->shifter_operand = immed_8;
        retval->shifter_operand = rotate_right(retval->shifter_operand, rotate_imm);
        if (rotate_imm == 0) {
            retval->shifter_carry = get_bit(state.CPSR, CPSR_C);
        } else {
            retval->shifter_carry = get_bit(retval->shifter_operand, 31);
        }
    } else {
        s_type = (instruction >> 4) & 0x7; //bits 6-4
        uint32_t Rm = state.regs[instruction & 0xF]; //bits 3-0.
        switch (s_type) {
            case LSLIMM :
                {
                    uint8_t shift_imm = (instruction >> 7) & 0x1F; //bits 11-7
                    retval->shifter_operand = Rm << shift_imm;
                    if (shift_imm == 0) {
                        retval->shifter_carry = get_bit(state.CPSR, CPSR_C);
                    } else {
                        retval->shifter_carry = get_bit(retval->shifter_operand, 32 - shift_imm);
                    }
                    break;
                }
            case LSLREG:
                {
                    uint8_t reg_id = (instruction >> 8) & 0xF; //bits 11-8.
                    uint8_t shift = state.regs[reg_id]; //take bits 7-0 of reg_id.
                    retval->shifter_operand = Rm << shift;
                    if (shift == 0) {
                        retval->shifter_carry = get_bit(state.CPSR, CPSR_C);
                    } else if (shift <= 32) {
                        retval->shifter_carry = get_bit(retval->shifter_operand, 32 - shift);
                    } else {
                        retval->shifter_carry = 0;
                    }
                    break;
                }
            case LSRIMM:
                {
                    uint8_t shift_imm = (instruction >> 7) & 0x1F; //bits 11-7
                    retval->shifter_operand = Rm >> shift_imm;
                    if (shift_imm == 0) {
                        retval->shifter_carry = get_bit(Rm, 31);
                    } else {
                        retval->shifter_carry = get_bit(Rm, shift_imm - 1);
                    }
                    break;
                }
            case LSRREG:
                {
                    uint8_t reg_id = (instruction >> 8) & 0xF; //bits 11-8.
                    uint8_t shift = state.regs[reg_id]; //take bits 7-0 of reg_id.
                    retval->shifter_operand = Rm >> shift;
                    if (shift == 0) {
                        retval->shifter_carry = get_bit(state.CPSR, CPSR_C);
                    } else if (shift <= 32) {
                        retval->shifter_carry = get_bit(retval->shifter_operand, shift - 1);
                    } else {
                        retval->shifter_carry = 0;
                    }
                    break;
                }
            case ASRIMM:
                {
                    uint8_t shift_imm = (instruction >> 7) & 0x1F; //bits 11-7
                    if (shift_imm == 0) {
                        retval->shifter_carry = get_bit(Rm, 31);
                        if (get_bit(Rm, 31) == 0) {
                            retval->shifter_operand = 0;
                        } else {
                            retval->shifter_operand = 0xFFFFFFFF;
                        }
                    } else {
                        retval->shifter_operand = arithmetic_right_shift(Rm, shift_imm);
                        retval->shifter_carry = get_bit(Rm, shift_imm - 1);
                    }
                    break;
                }
            case ASRREG:
                {
                    uint8_t reg_id = (instruction >> 8) & 0xF; //bits 11-8.
                    uint8_t shift = state.regs[reg_id]; //take bits 7-0 of reg_id.
                    if (shift <= 32) {
                        retval->shifter_operand = arithmetic_right_shift(Rm, shift);
                        if (shift == 0) {
                            retval->shifter_carry = get_bit(state.CPSR, CPSR_C);
                        } else {
                            retval->shifter_carry = get_bit(Rm, shift - 1);
                        }
                    } else {
                        retval->shifter_carry = get_bit(Rm, 31);
                        if (get_bit(Rm, 31) == 0) {
                            retval->shifter_operand = 0;
                        } else {
                            retval->shifter_operand = 0xFFFFFFFF;
                        }
                    }
                    break;
                }
            case RORIMM:
                {
                    uint8_t shift_imm = (instruction >> 7) & 0x1F; //bits 11-7
                    if (shift_imm == 0) { //rotate right with extend.
                        retval->shifter_operand = (Rm >> 1) | ((uint32_t)get_bit(state.CPSR, CPSR_C) << 31);
                        retval->shifter_carry = get_bit(Rm, 0);
                    } else {
                        retval->shifter_operand = rotate_right(Rm, shift_imm);
                        retval->shifter_carry = get_bit(Rm, 31);
                    }
                    break;
                }
            case RORREG:
                {
                    uint8_t reg_id = (instruction >> 8) & 0xF; //bits 11-8.
                    uint8_t shift = state.regs[reg_id]; //take bits 7-0 of reg_id.
                    if (shift == 0) {
                        retval->shifter_operand = Rm;
                        retval->shifter_carry = get_bit(state.CPSR, CPSR_C);
                    } else if ((shift & 0xF) == 0) { //bits 4-0 are 0.
                        retval->shifter_operand = Rm;
                        retval->shifter_carry = get_bit(Rm, 31);
                    } else {
                        retval->shifter_operand = rotate_right(Rm, shift & 0xF);
                        retval->shifter_carry = get_bit(Rm, (shift & 0xF) - 1);
                    }
                    break;
                }
        }
    }
    return retval;
}

uint32_t ld_str_addr_mode(struct CPUState curr_state,
                          struct CPUState *next_state, uint32_t instruction)
{
    enum ShifterType {
        LSL = 0x00, // logical shift left
        LSR = 0x01, // logical shift right
        ASR = 0x10, // arithmetic shift right
        ROR = 0x11, // rotate right (optionally with extend RRX)
    } s_type;
    uint32_t address;
    uint32_t offset;
    uint8_t rn_id = (instruction >> 16) & 0xf; // bits 19-16
    int32_t rn_val = curr_state.regs[rn_id];

    if (instruction & (1 << I_BIT)) { // Immediate offset
        offset = instruction & 0x7ff; // bits 11-0
    } else { // Register offset
        uint8_t rm_id = instruction & 0xf; // bits 3-0
        int32_t rm_val = curr_state.regs[rm_id];
        uint8_t shift_imm = (instruction >> 7) & 0x1f; // bits 11-7
        s_type = (instruction >> 5) & 0x3; // bits 6-5
        switch (s_type) {
            case LSL:
            {
                offset = rm_val << shift_imm; // this also covers unscaled case
                break;
            }
            case LSR:
            {
                if (shift_imm == 0) { // shift_imm is 32
                    offset = 0;
                } else {
                    offset = ((uint32_t) rm_val) >> shift_imm;
                }
                break;
            }
            case ASR:
            {
                if (shift_imm == 0) { // shift_imm is 32
                    if (get_bit(rm_val, 31) == 1) {
                        offset = 0xffffffff;
                    } else {
                        offset = 0;
                    }
                } else {
                    offset = arithmetic_right_shift(rm_val, shift_imm);
                }
                break;
            }
            case ROR:
            {
                if (shift_imm == 0) { // RRX rotate right with extend
                    offset = (((uint32_t) get_bit(curr_state.CPSR, CPSR_C)) << 31) |
                             (((uint32_t) rm_val) >> 1);
                } else {
                    offset = rotate_right(rm_val, shift_imm);
                }
                break;
            }
        }
    }

    if (instruction & (1 << U_BIT)) {
        address = rn_val + offset;
    } else {
        address = rn_val - offset;
    }

    uint32_t ret_val;

    const uint8_t P = get_bit(instruction, P_BIT), W = get_bit(instruction, W_BIT);

    if (!P && !W) { // post-indexed
        ret_val = rn_val;
        if (condition_check(curr_state, (uint8_t) (instruction >> 28))) {
            next_state->regs[rn_id] = address;
        }
    } else if (!P && W) { // user mode access, we are not implementing this
        next_state->halted = 1;
        ret_val = address;
    } else if (P && !W) { // normal
        ret_val = address;
    } else { // (P = 1, W = 1) pre-indexed
        ret_val = address;
        if (condition_check(curr_state, (uint8_t) (instruction >> 28))) {
            next_state->regs[rn_id] = address;
        }
    }

    return ret_val;
}

uint32_t arithmetic_right_shift(uint32_t shiftee, uint8_t shifter)
{
#define SIGN_BIT 31
    uint32_t sign_bit = get_bit(shiftee, SIGN_BIT);
    if (sign_bit == 0) { //just ordinary right shifting
        return (shiftee >> shifter);
    } else { //shift in 1s on the left.
        const uint32_t one = 1;
        //evil bitwise magic incoming.
        return (shiftee >> shifter) | ~((one << (32 - shifter)) - 1);
        //yah no this isn't that hard to understand
        //so we take 1 and move it to the first new bit
        //example if 1000`0000`0000`0001`1000`1111`1000`0011`
        //was shifted to the right twice, the right most "new" bit 
        //would be at index 32 - 2
        //so we make mask like 0100`0000`0000`0000`0000`0000`0000`0000`
        //subtract 1 like `0011`1111`1111`1111`1111`1111`1111`1111
        //take complement et voila `1100`0000`0000`0000`0000`0000`0000`0000
    }
#undef SIGN_BIT
}

bool condition_check(struct CPUState curr_state, uint8_t cond)
{
    enum Condition {
        EQ = 0x00,
        NE = 0x01,
        CS = 0x02,
        CC = 0x03,
        MI = 0x04,
        PL = 0x05,
        VS = 0x06,
        VC = 0x07,
        HI = 0x08,
        LS = 0x09,
        GE = 0x0A,
        LT = 0x0B,
        GT = 0x0C,
        LE = 0x0D,
        AL = 0x0E
    };

    switch(cond){
        case EQ: 
            {
                return (curr_state.CPSR & (1 << CPSR_Z));
                break;
            }
        case NE: 
            {
                return (!(curr_state.CPSR & (1 << CPSR_Z)));
                break;
            }
        case CS: 
            {
                return (curr_state.CPSR & (1 << CPSR_C));
                break;
            }
        case CC: 
            {
                return (!(curr_state.CPSR & (1 << CPSR_C)));
                break;
            }
        case MI: 
            {
                return (curr_state.CPSR & (1 << CPSR_N));
                break;
            }case PL: 
            {
                return (!(curr_state.CPSR & (1 << CPSR_N)));
                break;
            }
        case VS: 
            {
                return (curr_state.CPSR & (1 << CPSR_V));
                break;
            }
        case VC: 
            {
                return (!(curr_state.CPSR & (1 << CPSR_V)));
                break;
            }
        case HI: 
            {
                return ( 
                        (curr_state.CPSR & (1 << CPSR_C)) && 
                        !(curr_state.CPSR & (1 << CPSR_Z)) 
                       );
                break;
            }
        case LS: 
            {
                return ( 
                        !(curr_state.CPSR & (1 << CPSR_C)) || 
                        (curr_state.CPSR & (1 << CPSR_Z)) 
                       );
                break;
            }
        case GE: 
            {
                return ( 
                        ((curr_state.CPSR & (1 << CPSR_N)) && (curr_state.CPSR & (1 << CPSR_V))) || 
                        (!(curr_state.CPSR & (1 << CPSR_N)) && !(curr_state.CPSR & (1 << CPSR_V)))
                       );
                break;
            }
        case LT: 
            {
                return ( 
                        (!(curr_state.CPSR & (1 << CPSR_N)) && (curr_state.CPSR & (1 << CPSR_V))) || 
                        ((curr_state.CPSR & (1 << CPSR_N)) && !(curr_state.CPSR & (1 << CPSR_V)))
                       );
                break;
            }
        case GT: 
            {
                return ( 
                        ( !(curr_state.CPSR & (1 << CPSR_Z)) ) && 
                        ( ( (curr_state.CPSR & (1 << CPSR_N)) && (curr_state.CPSR & (1 << CPSR_V)) ) || ( !(curr_state.CPSR & (1 << CPSR_N)) && !(curr_state.CPSR & (1 << CPSR_V)) ) )
                       );
                break;
            }
        case LE: 
            {
                return ( 
                        ( (curr_state.CPSR & (1 << CPSR_Z)) ) && 
                        ( ( !(curr_state.CPSR & (1 << CPSR_N)) && (curr_state.CPSR & (1 << CPSR_V)) ) || ( (curr_state.CPSR & (1 << CPSR_N)) && !(curr_state.CPSR & (1 << CPSR_V)) ) )
                       );
                break;
            }
        case AL: 
            {
                return true;
                break;
            }
        default: return false;
    }
}

uint32_t sign_extend(uint32_t num, uint8_t curr_width, uint8_t req_width)
{
    const uint32_t one = 1;
    if (req_width > 32) {
        fprintf(stderr, "Error: requirements too wide, I'm not even going to try");
        return num;
    }
    if (req_width < curr_width) {
        fprintf(stderr, "Warning: curr_width(=%d) > req_width(=%d), may lead to overflow", curr_width, req_width);
        num &= ((one << req_width) - 1);
        return num;
    } else if (req_width == curr_width) {
        return num;
    } else {
        if (num & (one << (curr_width - 1))) { //negative
            uint32_t mask_big = (one << req_width) - 1;
            uint32_t mask_small = (one << curr_width) - 1;
            num |= (mask_big ^ mask_small);
            return num;
        } else { //not negative.
            uint32_t mask_small = (one << curr_width) - 1;
            num &= mask_small;
            return num;
        }
    }
}
