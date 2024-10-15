PARSER=parser
ASSEMBLER=assembler
LINKER=linker
EMULATOR=emulator

.\/${PARSER} ./tests/main.s
.\/${ASSEMBLER} -o main.o parser_output.txt
.\/${PARSER} ./tests/math.s
.\/${ASSEMBLER} -o math.o parser_output.txt
.\/${PARSER} ./tests/handler.s
.\/${ASSEMBLER} -o handler.o parser_output.txt
.\/${PARSER} ./tests/isr_timer.s
.\/${ASSEMBLER} -o isr_timer.o parser_output.txt
.\/${PARSER} ./tests/isr_terminal.s
.\/${ASSEMBLER} -o isr_terminal.o parser_output.txt
.\/${PARSER} ./tests/isr_software.s
.\/${ASSEMBLER} -o isr_software.o parser_output.txt
.\/${LINKER} -hex \
  -place=my_code@0x40000000 -place=math@0xF0000000 \
  -o program.hex \
  handler.o math.o main.o isr_terminal.o isr_timer.o isr_software.o
.\/${EMULATOR} program.hex