all: clean i8080

i8080:
	gcc i8080.c disassembler.c util.c instrs/arithmetic.c instrs/branching.c instrs/logical.c instrs/dataTransfer.c instrs/stack.c -g -o i8080


disassembler:
	gcc disassembler.c -g -o disassembler

clean-instrs:
	rm -f instrs/stack
	rm -f instrs/dataTransfer
	rm -f instrs/logical
	rm -f instrs/branching
	rm -f instrs/arithmetic

clean: clean-instrs
	rm -f util
	rm -f disassembler
	rm -f i8080
	rm -rf i8080.dSYM
