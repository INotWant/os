#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/** 代码段选择器 **/
#define KERNEL_CS 0x08

/** idt **/
typedef struct {
    uint16_t low_offset;    /* 操作函数的低 16 位 */
    uint16_t sel;           /* 段选择器 */
    uint8_t always0;
    uint8_t flags;
                            /**
                             * bit 7: "Interrupt is present"
                             * bits 6-5: Privilege level of caller (0=kernel..3=user)
                             * bit 4: Set to 0 for interrupt gates
                             * bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate
                             */
    uint16_t high_offset;   /* 操作函数的高 16 位 */
} __attribute__((packed)) idt_gate_t;   /* 'packed' gcc 选项，告诉编译器取消优化对齐 */

/** idtr(Interrupt Descriptor Table Register) **/
typedef struct {
    uint16_t limit;         /* 长度 */
    uint32_t base;          /* 基址 */
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
extern idt_gate_t idt[IDT_ENTRIES];
extern idt_register_t idt_reg;

/**
 * 修改 IDT
 * @param: n --> 第 n 个
 * @param: handler --> handle 函数地址
 */
void set_idt_gate(int n, uint32_t handler);

/**
 * 设置 idtr 寄存器（Load Interrupt Descriptor Table Register）
 */
void set_idt();

#endif