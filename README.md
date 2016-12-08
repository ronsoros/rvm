# rvm
simple virtual machine and assembler

----
compile:

    gcc -std=gnu99 vm.c -o vm
    gcc -std=gnu99 asm.c -o asm
    gcc -std=gnu99 rcc.c -o rcc

or just do "make"

usage:

    ./vm [name of binary]
    ./asm [name of source file] > [output binary]
    ./rcc [name of C file] > [output assembly source file]

sample programs are in test_asm