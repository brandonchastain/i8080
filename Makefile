all: clean i8080

i8080:
	gcc i8080.c disassembler.c util.c arithmetic.c branching.c logical.c dataTransfer.c stack.c -g -o i8080

disassembler:
	gcc disassembler.c -g -o disassembler

clean:
	rm -f stack
	rm -f dataTransfer
	rm -f logical
	rm -f branching
	rm -f arithmetic
	rm -f util
	rm -f disassembler
	rm -f i8080
	rm -rf i8080.dSYM
