#ifndef ISA_H
#define ISA_H

#include <stdlib.h>
#include <stdbool.h>
#include "sim.h"

/** Process instruction @ PC and increment PC by 4.
 * \param state Current state of CPU
 * \return New state of CPU
 */
struct CPUState process_instruction(struct CPUState state);

struct ShifterOperand {
    uint32_t shifter_operand;
    uint8_t shifter_carry;
};

/**
 * @brief Given instruction in ARM format, returns the shifter_operand and shifter_carry.
 *
 * @param state current state of machine, required when returning carry flags or to read registers.
 * @param instruction the ARM format instruction. The lowest 12 bits define the shifter operand stuff.
 *
 * @return 
 */
static struct ShifterOperand * shifter_operand(struct CPUState state, uint32_t instruction);
uint32_t rotate_right(uint32_t shiftee, uint8_t shifter);
uint32_t arithmetic_right_shift(uint32_t shiftee, uint8_t shifter);
uint8_t get_bit(uint32_t from, uint8_t bitid);

#define CPSR_N 31
#define CPSR_Z 30
#define CPSR_C 29
#define CPSR_V 28

#endif
