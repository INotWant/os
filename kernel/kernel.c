#include "kernel.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "lisp/pair.h"
#include "lisp/stack.h"

#include "../test/test.h"

void kernel_main() {
    interrupt_install();
    irq_install();

    memory_init();
    pair_init();
    stack_init();

    /* 调用所有测试 */
    test_main();

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