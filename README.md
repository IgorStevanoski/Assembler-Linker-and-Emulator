# Assembler, Linker and Emulator
- The goal of this project is implementation of translation tools and an emulator for an abstract computer system. The translation tools include an assembler for the abstract computer system and an arhitecture-independent linker. 
- The assembler takes the assembly source code (.s files) as input, which it translates into object (.o) files. The linker then links these files into a .hex file, and the emulator executes them.

## Lexical Analysis
- For the implementation of this project, programs for lexical analysis and parsing of input textual data are required. The tools used are [flex](https://en.wikipedia.org/wiki/Flex_(lexical_analyser_generator)) and [bison](https://en.wikipedia.org/wiki/GNU_Bison), which are generators for lexers and parsers, respectively.

## Assembler
- A two-pass assembler has been implemented in this project. The input to the assembler is the assembly source code (.s file). The output of the assembler is an object (.o) file. The format of the object file is based on a simplified version of the ELF format.
- The assembler is mostly implemented in the `assembler.cpp` file. The result of compiling this implementation is an `assembler` executable, which is used to assemble a single input file. The way to run it:

      assembler [options] <input_file_name>


## Linker
- An architecture-independent linker has been implemented in the project, which links one or more object files generated by the assembler, based on metadata (symbol tables, relocations, etc.). The input to the linker is the assembler’s output, and it is possible to specify multiple object programs to be linked. The output of the linker is a .hex file. 
- The linker is mostly implemented in the `linker.cpp` file. The result of compiling this implementation is a `linker` executable, which is used to link one or more input files. The way to run it:

      linker [options] <input_file_name>...

## Emulator
- An interpreter-emulator has been implemented in the project. The input to the emulator is a .hex file, which is the output of the linker. After the emulation is complete, the state of the emulated processor is printed.

- The emulator is mostly implemented in the `emulator.cpp` file. The result of compiling this implementation is an `emulator` executable, which is used to emulate the execution of a program from the input file. The way to run it:

      emulator <input_file_name>

## Example
- `handler.s`
```
.extern isr_timer, isr_terminal, isr_software

.global handler
.section my_handler
handler:
    push %r1
    push %r2
    csrrd %cause, %r1
    ld $2, %r2
    beq %r1, %r2, handle_timer
    ld $3, %r2
    beq %r1, %r2, handle_terminal
    ld $4, %r2
    beq %r1, %r2, handle_software
finish:
    pop %r2
    pop %r1
    iret
# obrada prekida od tajmera
handle_timer:
    call isr_timer
    jmp finish
# obrada prekida od terminala
handle_terminal:
    call isr_terminal
    jmp finish
# obrada softverskog prekida
handle_software:
    call isr_software
    jmp finish
    
.end
```
- parser output:
```
Directive: .extern Symbols: isr_timer isr_terminal isr_software
Directive: .global Symbols: handler
Directive: .section Section: my_handler
Label: handler
Instruction: push Operand: %r1
Instruction: push Operand: %r2
Instruction: csrrd Operand: %cause %r1
Instruction: ld Operand: $2 %r2
Instruction: beq Operand: %r1 %r2 handle_timer
Instruction: ld Operand: $3 %r2
Instruction: beq Operand: %r1 %r2 handle_terminal
Instruction: ld Operand: $4 %r2
Instruction: beq Operand: %r1 %r2 handle_software
Label: finish
Instruction: pop Operand: %r2
Instruction: pop Operand: %r1
Instruction: iret
Label: handle_timer
Instruction: call Operand: isr_timer
Instruction: jmp Operand: finish
Label: handle_terminal
Instruction: call Operand: isr_terminal
Instruction: jmp Operand: finish
Label: handle_software
Instruction: call Operand: isr_software
Instruction: jmp Operand: finish
Directive: .end
Done!!!
```
- `handler.o` (assembler output):
```
#.symtab
Num Value    Size Type  Bind Ndx Name
 0. 00000000    0 NOTYP  LOC UND  
 1. 00000000    0  SCTN  LOC   1 my_handler
 2. 00000000    0 NOTYP GLOB   1 handler
 3. 00000024    0 NOTYP  LOC   1 finish
 4. 00000030    0 NOTYP  LOC   1 handle_timer
 5. 00000038    0 NOTYP  LOC   1 handle_terminal
 6. 00000040    0 NOTYP  LOC   1 handle_software
 7. 00000000    0 NOTYP GLOB UND isr_timer
 8. 00000000    0 NOTYP GLOB UND isr_terminal
 9. 00000000    0 NOTYP GLOB UND isr_software
#my_handler
81 e0 1F FC  81 e0 2F FC
90 12 00 00  91 20 00 01
31 01 20 04  91 20 00 02
31 01 20 05  91 20 00 03
31 01 20 06  93 2e 00 04
93 1e 00 04  93 fe 00 04
97 0e 00 04  20 00 00 07
30 00 00 03  20 00 00 08
30 00 00 03  20 00 00 09
30 00 00 03  
#my_handler.literal_pool
 0. 00000000
 1. 00000002
 2. 00000003
 3. 00000004
```
- `program.hex` (linker output):
```
40000000: 91 e0 00 04 91 10 10 02
40000008: 94 11 00 00 10 00 00 00
40000010: 91 10 00 05 81 e0 1F FC
40000018: 91 10 00 05 81 e0 1F FC
40000020: 20 00 00 06 80 03 10 13
40000028: 91 10 00 01 81 e0 1F FC
40000030: 91 10 00 05 81 e0 1F FC
40000038: 20 10 00 0b 80 03 10 14
40000040: 91 10 00 07 81 e0 1F FC
40000048: 91 10 00 0d 81 e0 1F FC
40000050: 20 10 00 0c 80 03 10 15
40000058: 91 10 00 09 81 e0 1F FC
40000060: 91 10 00 0e 81 e0 1F FC
40000068: 20 10 00 0e 80 03 10 16
40000070: 91 10 00 03 81 e0 1F FC
40000078: 91 10 00 0f 81 e0 1F FC
40000080: 20 10 00 0e 80 03 10 17
40000088: 91 10 30 12 91 20 30 13
40000090: 91 30 30 14 91 40 30 15
40000098: 91 50 30 16 91 60 30 17
400000a0: 91 70 30 18 00 00 00 00
f0000000: 81 e0 2F FC 92 1e 00 08
f0000008: 92 2e 00 0c 50 01 20 00
f0000010: 93 2e 00 04 93 fe 00 04
f0000018: 81 e0 2F FC 92 1e 00 08
f0000020: 92 2e 00 0c 51 01 20 00
f0000028: 93 2e 00 04 93 fe 00 04
f0000030: 81 e0 2F FC 92 1e 00 08
f0000038: 92 2e 00 0c 52 01 20 00
f0000040: 93 2e 00 04 93 fe 00 04
f0000048: 81 e0 2F FC 92 1e 00 08
f0000050: 92 2e 00 0c 53 01 20 00
f0000058: 93 2e 00 04 93 fe 00 04
f0000060: 81 e0 1F FC 81 e0 2F FC
f0000068: 90 12 00 00 91 20 00 01
f0000070: 31 11 20 04 91 20 00 02
f0000078: 31 11 20 05 91 20 00 03
f0000080: 31 11 20 06 93 2e 00 04
f0000088: 93 1e 00 04 93 fe 10 04
f0000090: 97 0e 00 04 20 10 00 07
f0000098: 30 10 00 03 20 10 00 08
f00000a0: 30 10 00 03 20 10 00 09
f00000a8: 30 10 00 03 00 00 00 00
f00000b0: 00 00 00 00 00 00 00 00
f00000b8: 00 00 00 00 00 00 00 00
f00000c0: 00 00 00 00 00 00 00 00
f00000c8: 93 fe 00 04 93 fe 00 04
f00000d0: 81 e0 1F FC 81 e0 2F FC
f00000d8: 91 10 00 0c 91 20 10 12
f00000e0: 82 20 10 00 93 2e 00 04
f00000e8: 93 1e 00 04 93 fe 00 04
f00000f0: 00 00 00 00 60 00 00 f0
f00000f8: 60 00 00 f0 84 00 00 f0
f0000100: 94 00 00 f0 9c 00 00 f0
f0000108: a4 00 00 f0 cc 00 00 f0
f0000110: c8 00 00 f0 d0 00 00 f0
f0000118: 00 00 00 f0 00 00 00 f0
f0000120: 18 00 00 f0 30 00 00 f0
f0000128: 48 00 00 f0 00 00 00 40
f0000130: 00 00 00 40 ac 00 00 f0
f0000138: ac 00 00 f0 b0 00 00 f0
f0000140: b4 00 00 f0 b8 00 00 f0
f0000148: bc 00 00 f0 c0 00 00 f0
f0000150: c4 00 00 f0 c8 00 00 f0
f0000158: 00 00 00 00 02 00 00 00
f0000160: 03 00 00 00 04 00 00 00
f0000168: fe fe ff ff 01 00 00 00
f0000170: 00 00 00 f0 07 00 00 00
f0000178: 32 00 00 00 05 00 00 00
f0000180: 13 00 00 00 12 00 00 00
f0000188: cd ab 00 00 0b 00 00 00
f0000190: 19 00 00 00 18 00 00 00
f0000198: f0 00 00 f0 f0 00 01 58
```
- emulator output:
```
Emulated processor executed halt instruction
Emulated processor state:
 r0=0x00000000    r1=0x0000abcd    r2=0x00000002    r3=0x00000003
 r4=0x00000004    r5=0x00000005    r6=0x00000006    r7=0x00000000
 r8=0x00000000    r9=0x00000000   r10=0x00000000   r11=0x00000000
r12=0x00000000   r13=0x00000000   r14=0xfffffed6   r15=0x400000a4
```
  
### Note: 
- The project is developed and run in a virtual machine with a configured environment and tools. It is executed by running the corresponding script:

      bash shell.sh
- More details about the processor for which the assembler was written, the corresponding syntax, and the computer system for which the emulator was implemented can be found in the project documentation.
