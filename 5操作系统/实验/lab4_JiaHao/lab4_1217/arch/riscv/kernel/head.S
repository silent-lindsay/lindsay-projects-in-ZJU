    .extern start_kernel
    .extern task_init
    .extern mm_init
    .section .text.init
    .globl _start
_start:
    la sp, boot_stack_top
    call setup_vm
    call relocate
    call mm_init
    call setup_vm_final
    call task_init

    # set stvec = _traps
    la t0, _traps
    csrw stvec, t0

    # set sie[STIE] = 1
    li t1, 0x20
    csrs sie, t1
    
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
    
    # set sstatus[SIE] = 1
    li t1, 0x2
    csrs sstatus, t1




    call start_kernel
    
relocate:
    # set ra = ra + PA2VA_OFFSET
    # set sp = sp + PA2VA_OFFSET (If you have set the sp before)
    li t0,0xFFFFFFDF80000000

    add ra, ra, t0
    add sp, sp, t0

    # set satp with early_pgtbl
    li t0,0x8000000000000000
    li t1,0
    la t2,early_pgtbl
    srli t2,t2,12
    add t0,t0,t1
    add t0,t0,t2
    csrw satp,t0
    
    # flush tlb
    sfence.vma zero, zero

    ret

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top:
