#ifndef ISA_H
#define ISA_H

/** Process instruction @ PC and increment PC by 4.
 * \param state Current state of CPU
 * \return New state of CPU
 */
struct CPUState process_instruction(struct CPUState state);

#endif
