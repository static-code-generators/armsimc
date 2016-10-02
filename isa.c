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
	if(cond == 0x00)	//EQ
	{
		return (curr_state.CPSR & (1 << Z));
	}
	else if(cond == 0x01) 	//NE
	{
		return (!(curr_state.CPSR & (1 << Z)));
	}
	else if(cond == 0x02)	//CS/HS
	{
		return (curr_state.CPSR & (1 << C));
	}
	else if(cond == 0x03)	//CC/LO
	{
		return (!(curr_state.CPSR & (1 << C)));
	}
	else if(cond == 0x04)	//MI
	{
		return (curr_state.CPSR & (1 << N));
	}
	else if(cond == 0x05)	//PL
	{
		return (!(curr_state.CPSR & (1 << N)));
	}
	else if(cond == 0x06)	//VS
	{
		return (curr_state.CPSR & (1 << V));	
	}
	else if(cond == 0x07)	//VC
	{
		return (!(curr_state.CPSR & (1 << V)));
	}
	else if(cond == 0x08)	//HI
	{
		return ( 
			(curr_state.CPSR & (1 << C)) && 
			!(curr_state.CPSR & (1 << Z)) 
		);
	}
	else if(cond == 0x09)	//LS
	{
		return ( 
			!(curr_state.CPSR & (1 << C)) || 
			(curr_state.CPSR & (1 << Z)) 
		);
	}
	else if(cond == 0x0A)	//GE
	{
		return ( 
			((curr_state.CPSR & (1 << N)) && (curr_state.CPSR & (1 << V))) || 
			(!(curr_state.CPSR & (1 << N)) && !(curr_state.CPSR & (1 << V)))
		);
	}
	else if(cond == 0x0B)	//LT
	{
		return ( 
			(!(curr_state.CPSR & (1 << N)) && (curr_state.CPSR & (1 << V))) || 
			((curr_state.CPSR & (1 << N)) && !(curr_state.CPSR & (1 << V)))
		);
	}
	else if(cond == 0x0C)	//GT
	{
		return ( 
			( !(curr_state.CPSR & (1 << Z)) ) && 
			( ( (curr_state.CPSR & (1 << N)) && (curr_state.CPSR & (1 << V)) ) || ( !(curr_state.CPSR & (1 << N)) && !(curr_state.CPSR & (1 << V)) ) )
		);
	}
	else if(cond == 0x0D)	//LE
	{
		return ( 
			( (curr_state.CPSR & (1 << Z)) ) && 
			( ( !(curr_state.CPSR & (1 << N)) && (curr_state.CPSR & (1 << V)) ) || ( (curr_state.CPSR & (1 << N)) && !(curr_state.CPSR & (1 << V)) ) )
		);
	}
	else if(cond == 0x0E)	//AL
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct CPUState process_instruction(struct CPUState state)
{
    next_state = curr_state = state;
    next_state.regs[PC] += 4;
    // TODO: decode instruction @ PC and execute it
    return next_state;
}
