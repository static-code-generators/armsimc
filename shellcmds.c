#include <stdio.h>
#include "shellcmds.h"

void cmd_run(){}
void cmd_file(char *fname)
{
}

void cmd_step(int nbstep){}
void cmd_mdump(int low_addr, int high_addr, char *fname){}
void cmd_rdump(char *fname){}
void cmd_set(int reg_num, int reg_val){}

void cmd_help()
{
    printf("`r` or `run`: simulate the program until it indicates that the simulator should halt.\n");
    printf("`file <hexfile>`: load this file in program memory.\n");
    printf("`step [i]`: execute one instruction (or optionally `i`)\n");
    printf("`mdump <low> <high> [dumpfile]`: dump the contents of memory, from location low to location high to the screen or to the dump file [dumpfile].\n");
    printf("`rdump [dumpfile]`: dump the current instruction count, the contents of R0 – R14, R15 (PC), and the CPSR to the screen or to the file [dumpfile].\n");
    printf("`set <reg_num> <reg_val>`: set general purpose register reg num to value reg val.\n");
    printf("`?` or `help`: print out a list of all shell commands.\n");
    printf("`q` or `quit`: quit the shell.\n");
}

