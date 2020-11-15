#include "kernel.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"

void kernel_main() {
    interrupt_install();
    irq_install();

    memory_init();

    kprint("malloc:\n");
    char *p = (char *)memory_malloc(9);
    p[0] = 'I';
    p[1] = 'N';
    p[2] = 'o';
    p[3] = 't';
    p[4] = 'W';
    p[5] = 'a';
    p[6] = 'n';
    p[7] = 't';
    p[8] = '\0';
    kprint(p);
    memory_free(p);
    kprint("\n");
    kprint("free!\n");
    kprint("malloc:\n");
    p = (char *)memory_malloc(1);
    p[0] = 'Y';
    kprint(p);
    memory_free(p);
    kprint("\n");
    kprint("free!\n");

    kprint("Type something, it will go through the kernel.\n"
        "Type END to halt the CPU\n> ");
}

void user_input(char* input) {
    if (strcmp(input, "END") == 0){
        kprint("Stopping the CPU, Bye!\n");
        asm volatile("hlt");
    }
    kprint("You said: ");
    kprint(input);
    kprint("\n> ");
}