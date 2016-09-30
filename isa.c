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

struct CPUState process_instruction(struct CPUState state)
{
    next_state = curr_state = state;
    next_state.regs[PC] += 4;
    // TODO: decode instruction @ PC and execute it
    return next_state;
}
