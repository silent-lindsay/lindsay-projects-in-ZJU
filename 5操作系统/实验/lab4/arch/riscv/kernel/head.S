.extern start_kernel
.extern mm_init # in lab3
.extern task_init # in lab3
.extern setup_vm_final
.extern setup_vm
.extern early_pgtbl

    .section .text.init
    .globl _start
_start:
    la sp, boot_stack_top

    # lab4
    call setup_vm
    call relocate
    #######
    call mm_init # in lab3
    call setup_vm_final # lab4
    call task_init # lab3

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
    # la sp,boot_stack_top
    call start_kernel
    # - your code here -
    # ------------------

    # lab4
relocate:
    # set ra = ra + PA2VA_OFFSET
    # set sp = sp + PA2VA_OFFSET (If you have set the sp before)
    ######################
    # YOUR CODE HERE #
    li t0, 0xffffffdf80000000
    add ra,ra,t0
    add sp, sp, t0
    ######################
    # set satp with early_pgtbl
    ######################
    # YOUR CODE HERE #
    li t0, 0x8000000000000000
    la t1, early_pgtbl
    srli t1, t1, 12
    add t0, t0, t1 
    csrw satp, t0 
    # flush tlb
    sfence.vma zero, zero
    ret
    ######################

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top: