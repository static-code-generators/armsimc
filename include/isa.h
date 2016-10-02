#ifndef ISA_H
#define ISA_H

#include <stdbool.h>
#include "sim.h"

/** Process instruction @ PC and increment PC by 4.
 * \param state Current state of CPU
 * \return New state of CPU
 */
struct CPUState process_instruction(struct CPUState state);

#define N 31
#define Z 30
#define C 29
#define V 28

#endif
