// trap.c 
#include "defs.h"
#include "printk.h"

void trap_handler(unsigned long scause, unsigned long sepc) {
    // 通过 `scause` 判断trap类型
    // 如果是interrupt 判断是否是timer interrupt
    // 如果是timer interrupt 则打印输出相关信息, 并通过 `clock_set_next_event()` 设置下一次时钟中断
    // `clock_set_next_event()` 见 4.5 节
    // 其他interrupt / exception 可以直接忽略

    if ((scause & 0x8000000000000000) == 0x8000000000000000){
        unsigned long exception_code = scause & 0x7fffffffffffffff; // if scause[XLEN-2:0] == 5,it's a timer interrupt
        if (exception_code == 5){
            printk("kernel is running!\n");
            printk("[S] Supervisor Mode Timer Interrupt\n");
            clock_set_next_event();
        }
    }
    return;
}