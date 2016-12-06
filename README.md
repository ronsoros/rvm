# rvm
simple virtual machine and assembler

----
compile:

    gcc -std=gnu99 vm.c -o vm
    gcc -std=gnu99 asm.c -o asm

usage:

    ./vm [name of binary]
    ./asm [name of source file] > [output binary]

sample program: test.vas