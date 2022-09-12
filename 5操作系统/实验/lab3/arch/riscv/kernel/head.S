.extern start_kernel
.extern mm_init # in lab3
.extern task_init # in lab3

    .section .text.init
    .globl _start
_start:

    la sp, boot_stack_top
    call mm_init # in lab3

    la sp, boot_stack_top
    call task_init

    # stvec:0x105
    # sie:0x104
    # sstatus:0x100

# set stvec = _traps 
    la t0, _traps # t0 =_traps1
    csrw stvec, t0 
    
# set sie[STIE] = 1    
    addi t1, x0, 32
    csrs sie, t1 

# set first time interrupt
    rdtime a0
    li t0,10000000
    add a0,a0,t0
    addi a1,x0,0
    addi a2,x0,0
    addi a3,x0,0
    addi a4,x0,0
    addi a5,x0,0
    addi a6,x0,0
    addi a7,x0,0
    ecall 
# set sstatus[SIE] = 1
    csrsi sstatus, 2 

    # ------------------
    la sp,boot_stack_top
    call start_kernel
    # - your code here -
    # ------------------

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top: