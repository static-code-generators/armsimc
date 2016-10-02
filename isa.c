#include "sim.h"
#include "isa.h"


/* Instructions to implement:
 * ADC ADD AND B BIC
 * BL CMN CMP EOR LDR
 * LDRB MLA MOV MUL MVN
 * ORR RSB RSC SBC STR
 * STRB SUB TEQ TST SWI
 */

static struct CPUState next_state, curr_state;

bool condition_check(uint8_t cond)
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

struct CPUState process_instruction(struct CPUState state)
{
    next_state = curr_state = state;
    next_state.regs[PC] += 4;
    // TODO: decode instruction @ PC and execute it
    return next_state;
}

uint32_t rotate_right(uint32_t shiftee, uint8_t shifter)
{
    return (shiftee << (32 - shifter)) | (shiftee >> (shifter));
}

uint8_t get_bit(uint32_t from, uint8_t bitid)
{
    return (from >> bitid) & 1;
}

static struct ShifterOperand * shifter_operand(struct CPUState state, uint32_t instruction)
{
#define I_BIT 25
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
            retval->shifter_carry = get_bit(retval->shifter_carry, 31);
        }
    } else {
        s_type = (instruction >> 4) & 0x7; //bits 6-4
        uint32_t Rm = state.regs[instruction & 0xF]; //bits 3-0.
        switch (s_type) {
            case LSLIMM :
                {
                    uint8_t shift_imm = (instruction >> 7) & 0x2F; //bits 11-7
                    retval->shifter_operand = Rm << shift_imm;
                    if (shift_imm == 0) {
                        retval->shifter_carry = get_bit(state.CPSR, CPSR_C);
                    } else {
                        retval->shifter_carry = get_bit(retval->shifter_carry, 32 - shift_imm);
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
                        retval->shifter_carry = get_bit(retval->shifter_carry, 32 - shift);
                    } else {
                        retval->shifter_carry = 0;
                    }
                    break;
                }
            case LSRIMM:
                {
                    uint8_t shift_imm = (instruction >> 7) & 0x2F; //bits 11-7
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
                        retval->shifter_carry = get_bit(retval->shifter_carry, shift - 1);
                    } else {
                        retval->shifter_carry = 0;
                    }
                    break;
                }
            case ASRIMM:
                {
                    uint8_t shift_imm = (instruction >> 7) & 0x2F; //bits 11-7
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
                    uint8_t shift_imm = (instruction >> 7) & 0x2F; //bits 11-7
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
#undef I_BIT
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
