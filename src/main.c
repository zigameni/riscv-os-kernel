//
// Created by os on 8/21/25.
//


#include "../lib/console.h"

void main() {
    __putc('O');
    __putc('S');
    __putc('1');
    __putc('\n');
    __putc('\n');

    while(1){
        char character = __getc();
        __putc(character+30);
    }
}