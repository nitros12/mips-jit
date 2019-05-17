# mips-jit

A JIT is a type of interpreter/ emulator right?

# Building

Build using the provided makefile

# Usage

``` shell
./mips_jit <input file>
```


The program first prints the parsed MIPS instructions, then the intermediate
abstract instructions, the generated x86 instructions, then the assembled x86
instructions.
Then the program is run on the host machine, after running the state of the
registers are printed.

You will probably find that the $zero register is printed, however at the
abstract instruction stage all instances of theh register $zero are replaced
with immediate values of 0, the register still gets mapped anyway because it's
in the register list, and we map all x86 registers to a mips register
unconditionally, only stack mapped registers are conditionally allocated.
