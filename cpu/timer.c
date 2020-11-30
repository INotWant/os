#include "timer.h"
#include "ports.h"
#include "isr.h"

#include "../libc/utils.h"
#include "../drivers/screen.h"


void init_timer() {
    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / TIMER_FREQ;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)( (divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}

static uint32_t tick = 0;
static uint32_t sleep_time;
static uint8_t is_go = 0;

static void timer_callback(registers_t regs) {
    ++tick;
    if (tick >= sleep_time) {  /* 已到时间 */
        /* 恢复默认的定时器回调函数 */
        update_interrupt_handler(IRQ0, 0);
        is_go = 1;
    }
    UNUSED(regs);
}

void sleep(uint32_t time) {
    /* 心跳数置 0，并设置睡眠时间 */
    tick = 0;
    sleep_time = time;
    is_go = 0;
    /* 更新定时器的回调函数 */
    update_interrupt_handler(IRQ0, timer_callback);
    while (!is_go)
        asm volatile("hlt");
}