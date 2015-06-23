all: clean disassembler

disassembler:
	gcc disassembler.c -g -o disassembler

clean:
	rm -f disassembler
