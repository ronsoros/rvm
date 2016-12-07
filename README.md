# rvm
simple virtual machine and assembler

----
compile:

    gcc -std=gnu99 vm.c -o vm
    gcc -std=gnu99 asm.c -o asm
    gcc -std=gnu99 rcc.c -o rcc

usage:

    ./vm [name of binary]
    ./asm [name of source file] > [output binary]
    ./rcc [name of C file] > [output assembly source file]

sample program: test.vas, timertest.vas, test2.c