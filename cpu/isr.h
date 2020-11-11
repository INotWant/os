#ifndef ISR_H
#define ISR_H

#include <stdint.h>

/** 用于 CPU 异常的中断服务例程（内中断） **/
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr32();

/** 外围设备的中断请求（外中断） **/
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/** 外中断号 **/
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

/** 聚合多个寄存器（用于参数传递） **/
typedef struct {
    uint32_t ds;                                        
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;    /* pusha */
    uint32_t int_no, err_code;                          /* 中断号，错误码 */
    uint32_t eip, cs, eflags, useresp, ss;              /* 发生中断时处理器自动 push 的 */
} registers_t;

typedef void (*isr_t)(registers_t);

/**
 * 安装中断处理函数（含内外中断，外中断处理函数中只含有分派逻辑）
 */
void interrupt_install();

/**
 * 安装外中断处理函数
 */
void irq_install();

/**
 * 处理内中断的默认逻辑
 */
void isr_handler(registers_t r);

/**
 * 处理外中断的初始逻辑（执行分派）
 */
void irq_handler(registers_t r);

/**
 * 修改外中断的操作函数
 */
void update_interrupt_handler(uint8_t n, isr_t handler);

#endif
