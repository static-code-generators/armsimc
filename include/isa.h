#ifndef ISA_H
#define ISA_H

#include <stdlib.h>

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

struct ShifterOperand * shifter_operand(struct CPUState state, uint32_t instruction);
uint32_t rotate_right(uint32_t shiftee, uint8_t shifter);
uint8_t get_bit(uint32_t from, uint8_t bitid);

#endif
