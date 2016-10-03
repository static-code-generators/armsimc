#ifndef ISA_HELPER_H
#define ISA_HELPER_H

#include <stdlib.h>
#include <stdbool.h>
#include "sim.h"

#define CPSR_N 31
#define CPSR_Z 30
#define CPSR_C 29
#define CPSR_V 28

#define L_BIT 20
#define W_BIT 21
#define B_BIT 22
#define U_BIT 23
#define P_BIT 24
#define I_BIT 25

struct ShifterOperand {
    uint32_t shifter_operand;
    uint8_t shifter_carry;
};

/**
 * Given instruction in ARM format, returns the shifter_operand and shifter_carry.
 *
 * @param state current state of machine, required when returning carry
 * flags or to read registers.*
 *
 * @param instruction the ARM format instruction.
 * The lowest 12 bits define the shifter operand stuff.
 *
 * @return malloc-ed pointer to a ShifterOperand struct.
 * The caller should free it after use
 */
struct ShifterOperand * shifter_operand(struct CPUState state, uint32_t instruction);
uint32_t rotate_right(uint32_t shiftee, uint8_t shifter);
uint32_t arithmetic_right_shift(uint32_t shiftee, uint8_t shifter);
uint8_t get_bit(uint32_t from, uint8_t bitid);
uint32_t get_bits(uint32_t from, uint8_t msb_id, uint8_t lsb_id);

/** Handle addr_mode and I, P, U, W operands.
 * Refer to Section A5.2 in ARM manual.
 * Note: This function might update Rn value in next_state
 * depending upon pre/post indexed addressing.
 * \param curr_state current state of machine
 * \param next_state pointer to next state of machine, might get modified depending
 *                   upon pre/post-indexed addressing
 * \param instruction 32-bit instruction
 * \return 32-bit decoded address
 */
uint32_t ld_str_addr_mode(struct CPUState curr_state,
                          struct CPUState *next_state, uint32_t instruction);

/** Check if current instruction should be executed depending upon the <cond> bits
 * and CPSR state.
 * \param curr_state current state of machine
 * \param cond bits 31-28 in instruction
 * \return boolean, true if execute current instruction
 */
bool condition_check(struct CPUState curr_state, uint8_t cond);

#endif
