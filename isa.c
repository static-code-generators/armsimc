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
