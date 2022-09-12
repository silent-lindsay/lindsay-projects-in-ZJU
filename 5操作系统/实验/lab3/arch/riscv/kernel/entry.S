.extern dummy    # in lab3
    .section .text.entry
    .align 2
    .globl _traps
    
_traps:

    # YOUR CODE HERE
    # -----------
    # 1. save 32 registers and sepc to stack
    csrw sscratch, sp # save the initial sp in sscratch
    addi sp, sp, -33 * 8 # 一个寄存器占据8个字节

    sd x0, 0*8(sp)
    sd x1, 1*8(sp)
    sd x3, 3*8(sp)
    sd x4, 4*8(sp)
    sd x5, 5*8(sp)
    sd x6, 6*8(sp)
    sd x7, 7*8(sp)
    sd x8, 8*8(sp)
    sd x9, 9*8(sp)
    sd x10, 10*8(sp)
    sd x11, 11*8(sp)
    sd x12, 12*8(sp)
    sd x13, 13*8(sp)
    sd x14, 14*8(sp)
    sd x15, 15*8(sp)
    sd x16, 16*8(sp)
    sd x17, 17*8(sp)
    sd x18, 18*8(sp)
    sd x19, 19*8(sp)
    sd x20, 20*8(sp)
    sd x21, 21*8(sp)
    sd x22, 22*8(sp)
    sd x23, 23*8(sp)
    sd x24, 24*8(sp)
    sd x25, 25*8(sp)
    sd x26, 26*8(sp)
    sd x27, 27*8(sp)
    sd x28, 28*8(sp)
    sd x29, 29*8(sp)
    sd x30, 30*8(sp)
    sd x31, 31*8(sp)
    # RISCV不能直接从CSR写到内存, 需要csrr把CSR读取到通用寄存器，再从通用寄存器sd到内存
    csrrw s0, sscratch, x0 # load sscratch which is the initial sp in s0 and sd it
    csrr s1,  sepc
    sd s0, 2*8(sp)
    sd s1, 32*8(sp)
    # -----------
    # 2. call trap_handler
    csrr a0, scause
    csrr a1, sepc
    #la sp, trap_handler
    call trap_handler
    # -----------
    # 3. restore sepc and 32 registers (x2(sp) should be restore last) from stack
    ld s1, 32*8(sp)
    csrw sepc, s1 # restore sepc

    ld x0, 0*8(sp)
    ld x1, 1*8(sp)
    ld x3, 3*8(sp)
    ld x4, 4*8(sp)
    ld x5, 5*8(sp)
    ld x6, 6*8(sp)
    ld x7, 7*8(sp)
    ld x8, 8*8(sp)
    ld x9, 9*8(sp)
    ld x10, 10*8(sp)
    ld x11, 11*8(sp)
    ld x12, 12*8(sp)
    ld x13, 13*8(sp)
    ld x14, 14*8(sp)
    ld x15, 15*8(sp)
    ld x16, 16*8(sp)
    ld x17, 17*8(sp)
    ld x18, 18*8(sp)
    ld x19, 19*8(sp)
    ld x20, 20*8(sp)
    ld x21, 21*8(sp)
    ld x22, 22*8(sp)
    ld x23, 23*8(sp)
    ld x24, 24*8(sp)
    ld x25, 25*8(sp)
    ld x26, 26*8(sp)
    ld x27, 27*8(sp)
    ld x28, 28*8(sp)
    ld x29, 29*8(sp)
    ld x30, 30*8(sp)
    ld x31, 31*8(sp)

    ld x2, 2*8(sp) # restore sp(x2) last
    # -----------
    # 4. return from trap
    sret
    # -----------


    .globl __dummy
__dummy:
    la t0, dummy
    csrw sepc, t0
    sret
    # YOUR CODE HERE
    
    .globl __switch_to
__switch_to:
    # save state to prev process
    sd ra, 5*8(a0)
    sd sp, 6*8(a0)
    sd s0, 7*8(a0)
    sd s1, 8*8(a0)
    sd s2, 9*8(a0)
    sd s3, 10*8(a0)
    sd s4, 11*8(a0)
    sd s5, 12*8(a0)
    sd s6, 13*8(a0)
    sd s7, 14*8(a0)
    sd s8, 15*8(a0)
    sd s9, 16*8(a0)
    sd s10, 17*8(a0)
    sd s11, 18*8(a0)
    # YOUR CODE HERE
    # restore state from next process
    ld ra, 5*8(a1)
    ld sp, 6*8(a1)
    ld s0, 7*8(a1)
    ld s1, 8*8(a1)
    ld s2, 9*8(a1)
    ld s3, 10*8(a1)
    ld s4, 11*8(a1)
    ld s5, 12*8(a1)
    ld s6, 13*8(a1)
    ld s7, 14*8(a1)
    ld s8, 15*8(a1)
    ld s9, 16*8(a1)
    ld s10, 17*8(a1)
    ld s11, 18*8(a1)
    # YOUR CODE HERE
    ret
