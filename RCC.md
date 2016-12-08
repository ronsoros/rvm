# RCC

rcc is the simple C compiler included. it works ok, but has a few bugs:

1. function calls and declarations must have a space between function name and the parenthesis: abc(5) -> abc (5)
2. function declarations cannot define parameters - if you try, strange bugs occur
