# Shell for ARM C Simulator

The purpose of the shell is to provide the user with commands to control the execution of the simulator.
The shell accepts one or more program files as command line arguments and loads them into the memory
image. In order to extract information from the simulator, a file named dumpsim will be created to hold
information requested from the simulator. The shell supports the following commands:

1. `go`: simulate the program until it indicates that the simulator should halt. (As we define below, this is when a SWI instruction is executed with a value of 0x0A.)
2. `run` <n>: simulate the execution of the machine for n instructions.
3. `mdump` <low> <high>: dump the contents of memory, from location low to location high to the screen and the dump file (dumpsim).
4. `rdump`: dump the current instruction count, the contents of R0 – R14, R15 (PC), and the CPSR to the screen and the dump file (dumpsim).
5. `input reg num reg val`: set general purpose register reg num to value reg val.
6. `?`: print out a list of all shell commands.
7. `quit`: quit the shell.
