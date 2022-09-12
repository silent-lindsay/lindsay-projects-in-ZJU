// arch/riscv/kernel/proc.c
#include "proc.h"
#include "printk.h"
#include "types.h"
#include "defs.h"

extern void __dummy();
extern void __switch_to(struct task_struct *prev, struct task_struct *next);

struct task_struct *idle;           // idle process
struct task_struct *current;        // 指向当前运⾏线程的 `task_struct`
struct task_struct *task[NR_TASKS]; // 线程数组，所有的线程都保存在此

/*
    当我们的 OS run 起来时候，其本身就是⼀个线程 idle 线程，但是我们并没有为它设计好 task_struct 。所以
    第⼀步我们要为 idle 设置 task_struct 。并将 current , task[0] 都指向 idle 。
    为了⽅便起⻅，我们将 task[1] ~ task[NR_TASKS - 1] , 全部初始化， 这⾥和 idle 设置的区别在于要为这些
    线程设置 thread_struct 中的 ra 和 sp .
    在 _start 适当的位置调⽤ task_init
*/
void task_init()
{
    // 1. 调⽤ kalloc() 为 idle 分配⼀个物理⻚
    idle = (struct task_struct *) kalloc() ;
    // 2. 设置 state 为 TASK_RUNNING;
    idle->state = TASK_RUNNING ;
    // 3. 由于 idle 不参与调度 可以将其 counter / priority 设置为 0
    idle->counter = 0 ;
    idle->priority = 0 ;
    // 4. 设置 idle 的 pid 为 0
    idle->pid = 0 ;
    // 5. 将 current 和 task[0] 指向 idle
    current = idle ;
    task[0] = idle ;
    /* YOUR CODE HERE */

    // 1. 参考 idle 的设置, 为 task[1] ~ task[NR_TASKS - 1] 进⾏初始化
    for ( int i = 1 ; i < NR_TASKS ; i++ ) {
        uint64 point = kalloc() ;
        task[i] = (struct task_struct *) point ;
        task[i]->state = TASK_RUNNING ;
        task[i]->counter = 0 ;
        task[i]->priority = rand() ;
        task[i]->pid = i ;
        task[i]->thread.ra = __dummy ;
        task[i]->thread.sp = point + PGSIZE ;
    }
    // 2. 其中每个线程的 state 为 TASK_RUNNING, counter 为 0, priority 使⽤ rand() 来设置, pid 为该线程在线程数组中的下标。
        // 3. 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`,
        // 4. 其中 `ra` 设置为 __dummy （⻅ 4.3.2）的地址， `sp` 设置为 该线程申请的物理⻚的⾼地址
        /* YOUR CODE HERE */
    
    printk("...proc_init done!\n");
    // PrintTask();
}

void dummy()
{
    uint64 MOD = 1000000007;
    uint64 auto_inc_local_var = 0;
    int last_counter = -1;
    while (1)
    {
        if (last_counter == -1 || current->counter != last_counter)
        {
            last_counter = current->counter;
            auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
            printk("[PID = %d] is running. auto_inc_local_var = %d\n", current->pid,
                   auto_inc_local_var);
        }
    }
}

void switch_to(struct task_struct *next)
{
    struct task_struct *temp = current ;
    current = next ;
    if ( next != temp ) {
        __switch_to( temp , next ) ;
    }
    /* YOUR CODE HERE */
}

void do_timer(void)
{
    /* 1. 如果当前线程是 idle 线程 直接进⾏调度 */
    /* 2. 如果当前线程不是 idle 对当前线程的运⾏剩余时间减 1
            若剩余时间任然⼤于0 则直接返回 否则进⾏调度 */
    if ( current == idle ) {
        /* code */
        schedule() ;
    } else {
        current->counter -- ;
        if ( current->counter > 0 ) return ;
        schedule() ;
    }
    /* YOUR CODE HERE */
}

// for test
void PrintTask(){
    for ( int i = 1 ; i < NR_TASKS ; i++ ) 
        // printk("task=%d -- priority=%d -- count=%d\n",task[i]->pid,task[i]->priority,task[i]->counter) ;
        printk("SET [PID = %d PRIORITY = %d COUNTER = %d]\n",task[i]->pid,task[i]->priority,task[i]->counter) ;
}


void reset()
{
    for ( int i = 1 ; i < NR_TASKS ; i++ ) 
        task[i]->counter = rand() ;
    PrintTask() ;
}

#ifdef SJF
/*
* 4.3.5.1 短作业优先调度算法
    遍历线程指针数组task (不包括 idle ，即 task[0] )，在所有运⾏状态 ( TASK_RUNNING ) 下的线程运⾏剩
    余时间最少的线程作为下⼀个执⾏的线程。
    如果所有运⾏状态下的线程运⾏剩余时间都为0，则对 task[1] ~ task[NR_TASKS-1] 的运⾏剩余时间重新
    赋值 (使⽤ rand() ) ，之后再重新进⾏调度。
*/
void schedule(void)
{
    // PrintTask() ;
    struct task_struct* next = NULL ;
    int shortest = 11 ;
    for ( int i = 1 ; i < NR_TASKS ; i++ ) {
        if ( shortest > task[i]->counter && task[i]->counter != 0 ) {
            next = task[i] ;
            shortest = task[i]->counter ;
        }
    }
    if ( shortest == 11 ) {
        // for ( int i = 1 ; i < NR_TASKS ; i++ ) 
        //     task[i]->counter = rand() ;
        // PrintTask() ;
        reset() ;
        schedule() ;
        // for ( int i = 1 ; i < NR_TASKS ; i++ ) {
        //     if ( shortest > task[i]->counter && task[i]->counter != 0 ) {
        //         next = task[i] ;
        //         shortest = task[i]->counter ;
        //     }
        // }        
    } 
    else { 
        printk("switch to [PID = %d PRIORITY = %d COUNTER = %d]\n",next->pid,next->priority,next->counter) ;
        switch_to(next) ;
    }
    /* YOUR CODE HERE */
}

#else
/*
* 4.3.5.2 优先级调度算法
*/
void schedule(void)
{
    // PrintTask() ;
    struct task_struct* next = NULL ;
    int pre = 11 ;
    for ( int i = 1 ; i < NR_TASKS ; i++ ) {
        if ( pre > task[i]->priority && task[i]->counter != 0 ) {
            next = task[i] ;
            pre = task[i]->priority ;
        }
    }
    if ( pre == 11 ) {        
        // for ( int i = 1 ; i < NR_TASKS ; i++ ) 
        //     task[i]->counter = rand() ;
        // PrintTask() ;
        reset() ;
        schedule() ;
    } else {
        printk("switch to [PID = %d PRIORITY = %d COUNTER = %d]\n",next->pid,next->priority,next->counter) ;
        switch_to(next) ;
    }
}
#endif