    .section .text.entry
    .align 2
    .globl _traps 
    .global __dummy
    .globl __switch_to
_traps:
    # 1. save 32 registers and sepc to stack
    addi sp, sp, -264
    sd x0, 256(sp)
    sd x1, 248(sp)
    sd x2, 240(sp)
    sd x3, 232(sp)
    sd x4, 224(sp)
    sd x5, 216(sp)
    sd x6, 208(sp)
    sd x7, 200(sp)
    sd x8, 192(sp)
    sd x9, 184(sp)
    sd x10, 176(sp)
    sd x11, 168(sp)
    sd x12, 160(sp)
    sd x13, 152(sp)
    sd x14, 144(sp)
    sd x15, 136(sp)
    sd x16, 128(sp)
    sd x17, 120(sp)
    sd x18, 112(sp)
    sd x19, 104(sp)
    sd x20, 96(sp)
    sd x21, 88(sp)
    sd x22, 80(sp)
    sd x23, 72(sp)
    sd x24, 64(sp)
    sd x25, 56(sp)
    sd x26, 48(sp)
    sd x27, 40(sp)
    sd x28, 32(sp)
    sd x29, 24(sp)
    sd x30, 16(sp)
    sd x31, 8(sp)

    # read sepc into t0
    csrr t0, sepc
    sd t0, 0(sp)
    
    # 2. call trap_handler
    csrr a0, scause
    csrr a1, sepc
    call trap_handler
    
    # 3. restore sepc and 32 registers (x2(sp) should be restore last) from stack
    ld t0, 0(sp)
    csrw sepc, t0
    ld x31, 8(sp)
    ld x30, 16(sp)
    ld x29, 24(sp)
    ld x28, 32(sp)
    ld x27, 40(sp)
    ld x26, 48(sp)
    ld x25, 56(sp)
    ld x24, 64(sp)
    ld x23, 72(sp)
    ld x22, 80(sp)
    ld x21, 88(sp)
    ld x20, 96(sp)
    ld x19, 104(sp)
    ld x18, 112(sp)
    ld x17, 120(sp)
    ld x16, 128(sp)
    ld x15, 136(sp)
    ld x14, 144(sp)
    ld x13, 152(sp)
    ld x12, 160(sp)
    ld x11, 168(sp)
    ld x10, 176(sp)
    ld x9, 184(sp)
    ld x8, 192(sp)
    ld x7, 200(sp)
    ld x6, 208(sp)
    ld x5, 216(sp)
    ld x4, 224(sp)
    ld x3, 232(sp)
    ld x2, 240(sp)
    ld x0, 256(sp)
    ld x1, 248(sp)
    addi sp, sp, 264
    
    # 4. return from trap
    sret

__dummy:
    la t0, dummy
    csrw sepc, t0
    sret

__switch_to:
    addi t0,a0,40
    sd ra,0(t0)
    sd sp,8(t0)
    sd s0,16(t0)
    sd s1,24(t0)
    sd s2,32(t0)
    sd s3,40(t0)
    sd s4,48(t0)
    sd s5,56(t0)
    sd s6,64(t0)
    sd s7,72(t0)
    sd s8,80(t0)
    sd s9,88(t0)
    sd s10,96(t0)
    sd s11,104(t0)
    
    # a0 = tast_struct* current, a1 = task_struct * next
    addi t0,a1,40
    ld ra,0(t0)
    ld sp,8(t0)
    ld s0,16(t0)
    ld s1,24(t0)
    ld s2,32(t0)
    ld s3,40(t0)
    ld s4,48(t0)
    ld s5,56(t0)
    ld s6,64(t0)
    ld s7,72(t0)
    ld s8,80(t0)
    ld s9,88(t0)
    ld s10,96(t0)
    ld s11,104(t0)

    ret