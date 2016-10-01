#ifndef SHELLCMDS_H
#define SHELLCMDS_H

#include <stdint.h>

void cmd_run();
void cmd_file(char *fname);
void cmd_step(int nbstep);
void cmd_mdump(uint32_t low_addr, uint32_t high_addr, char *fname);
void cmd_rdump(char *fname);
void cmd_set(int reg_num, int reg_val);
void cmd_help();

#endif
