// arch/riscv/include/proc.h
#include "types.h"
#define NR_TASKS (1 + 31) // ⽤于控制 最⼤线程数量 （idle 线程 + 31 内核线程）
#define TASK_RUNNING 0    // 为了简化实验，所有的线程都只有⼀种状态
#define PRIORITY_MIN 1
#define PRIORITY_MAX 10
/* ⽤于记录 `线程` 的 `内核栈与⽤户栈指针` */
/* (lab3中⽆需考虑，在这⾥引⼊是为了之后实验的使⽤) */
struct thread_info
{
    uint64 kernel_sp;
    uint64 user_sp;
};
/* 线程状态段数据结构 */
struct thread_struct
{
    uint64 ra;
    uint64 sp;
    uint64 s[12];
};
/* 线程数据结构 */
struct task_struct
{
    struct thread_info *thread_info;
    uint64 state;    // 线程状态
    uint64 counter;  // 运⾏剩余时间
    uint64 priority; // 运⾏优先级 1最低 10最⾼
    uint64 pid;      // 线程id
    struct thread_struct thread;
};
/* 线程初始化 创建 NR_TASKS 个线程 */
void task_init();
/* 在时钟中断处理中被调⽤ ⽤于判断是否需要进⾏调度 */
void do_timer();
/* 调度程序 选择出下⼀个运⾏的线程 */
void schedule();
/* 线程切换⼊⼝函数*/
void switch_to(struct task_struct *next);
/* dummy funciton: ⼀个循环程序，循环输出⾃⼰的 pid 以及⼀个⾃增的局部变量*/
void dummy();