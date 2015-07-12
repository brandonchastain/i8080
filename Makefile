all: clean i8080

i8080:
	gcc i8080.c disassembler.c -g -o i8080

disassembler:
	gcc disassembler.c -g -o disassembler

clean:
	rm -f disassembler
	rm -f i8080
	rm -rf i8080.dSYM
