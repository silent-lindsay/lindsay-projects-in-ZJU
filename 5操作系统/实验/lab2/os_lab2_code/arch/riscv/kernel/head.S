.extern start_kernel

    .section .text.init
    .globl _start
_start:
    # YOUR CODE HERE

    # ------------------
    # set stvec = _traps
    la t0, _traps
    csrw stvec, t0
    # ------------------
    # set sie[STIE] = 1
    li t1, 0x20
    csrs sie, t1
    # ------------------
    # set first time interrupt
    rdtime a0
    li t0, 10000000
    add a0, a0, t0
    li a1, 0
    li a2, 0
    li a3, 0
    li a4, 0
    li a5, 0
    li a6, 0x0
    li a7, 0x0
    ecall
    # ------------------
    # set sstatus[SIE] = 1
    li t1, 0x2
    csrs sstatus, t1 

    # ------------------

    la sp, boot_stack_top
    call start_kernel
    

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top:
