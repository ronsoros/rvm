CC = gcc
FLAGS = -std=gnu99
all : rcc vm asm
rcc : rcc.c
	$(CC) $(FLAGS) rcc.c -o rcc
vm : vm.c
	$(CC) $(FLAGS) vm.c -o vm
asm : asm.c
	$(CC) $(FLAGS) asm.c -o rcc