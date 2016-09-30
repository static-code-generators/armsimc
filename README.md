# ARM Simulator in C

Resources: [18-447 CMU Introduction to Computer Architecture Labs](http://www.ece.cmu.edu/~ece447/s14/doku.php?id=labs)

## Shell Instructions

The purpose of the shell is to provide the user with commands to control the execution of the simulator.
The shell accepts one or more program files as command line arguments and loads them into the memory
image. In order to extract information from the simulator, a file named dumpsim will be created to hold
information requested from the simulator. The shell supports the following commands:

1. `r` or `run`: simulate the program until it indicates that the simulator should halt. (As we define below, this is when a SWI instruction is executed with a value of 0x0A.)
2. `file <hexfile>`: load this file in program memory.
3. `step [i]`: execute one instruction (or optionally `i`)
4. `mdump <low> <high> [dumpfile]`: dump the contents of memory, from location low to location high to the screen or to the dump file [dumpfile].
5. `rdump [dumpfile]`: dump the current instruction count, the contents of R0 – R14, R15 (PC), and the CPSR to the screen or to the file [dumpfile].
6. `set <reg_num> <reg_val>`: set general purpose register reg num to value reg val.
7. `?` or `help`: print out a list of all shell commands.
8. `q` or `quit`: quit the shell.

### Workflow

1. Small feature gets assigned to a person after group meeting
2. The person implements it in a separate branch and then sends PR to master
3. The PR needs review by maintainer of module within 12hrs, and if okayed by them, can be merged into master
4. If no reply on PR till 12hrs hours by maintainer, anyone can review it, and if no reply till 24hrs more, it can be merged into master
5. PR should have no merge conflicts, it's responsibility of the PR-sender to keep their branch updated to master

### Maintainers

shell/sim: darkapex
verilog: kharghoshal
