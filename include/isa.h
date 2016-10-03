#ifndef ISA_H
#define ISA_H

#include <stdlib.h>
#include <stdbool.h>
#include "sim.h"

#define CPSR_N 31
#define CPSR_Z 30
#define CPSR_C 29
#define CPSR_V 28

#define W_BIT 21
#define U_BIT 23
#define P_BIT 24
#define I_BIT 25


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
 * Given instruction in ARM format, returns the shifter_operand and shifter_carry.
 *
 * @param instruction the ARM format instruction.
 * The lowest 12 bits define the shifter operand stuff.
 *
 * @return malloc-ed pointer to a ShifterOperand struct.
 * The caller should free it after use
 */
static struct ShifterOperand * shifter_operand(uint32_t instruction);
static uint32_t rotate_right(uint32_t shiftee, uint8_t shifter);
static uint32_t arithmetic_right_shift(uint32_t shiftee, uint8_t shifter);
static uint8_t get_bit(uint32_t from, uint8_t bitid);

/** Handle addr_mode and I, P, U, W operands.
 * Refer to Section A5.2 in ARM manual.
 * Note: This function might update Rn value in next_state
 * depending upon pre/post indexed addressing.
 * \param instruction 32-bit instruction
 * \return 32-bit decoded address
 */
static uint32_t ld_str_addr_mode(uint32_t instruction);

/** Check if current instruction should be executed depending upon the <cond> bits
 * and CPSR state.
 * \param cond bits 31-28 in instruction
 * \return boolean, true if execute current instruction
 */
static bool condition_check(uint8_t cond);

#endif
