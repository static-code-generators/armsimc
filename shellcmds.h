#ifndef SHELLCMDS_H
#define SHELLCMDS_H

void cmd_run();
void cmd_file(char *fname);
void cmd_step(int nbstep);
void cmd_mdump(int low_addr, int high_addr, char *fname);
void cmd_rdump(char *fname);
void cmd_set(int reg_num, int reg_val);
void cmd_help();

#endif
