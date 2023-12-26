.section    .vhex.tlbh.pretext, "ax"
.global     _vhex_tlbh_sh7305

_vhex_tlbh_sh7305:
    ! Generate "CPU" context
    sts.l   pr, @-r15
    stc.l   spc, @-r15
    stc.l   ssr, @-r15
    sts.l   mach, @-r15
    sts.l   macl, @-r15
    stc.l   gbr, @-r15
    stc.l   sgr, @-r15
    mov.l   r14, @-r15
    mov.l   r13, @-r15
    mov.l   r12, @-r15
    mov.l   r11, @-r15
    mov.l   r10, @-r15
    mov.l   r9, @-r15
    mov.l   r8, @-r15
    stc.l   R7_BANK, @-r15
    stc.l   R6_BANK, @-r15
    stc.l   R5_BANK, @-r15
    stc.l   R4_BANK, @-r15
    stc.l   R3_BANK, @-r15
    stc.l   R2_BANK, @-r15
    stc.l   R1_BANK, @-r15
    stc.l   R0_BANK, @-r15

    /* Set BL=0, IMASK=15 */
    stc sr, r3
    mov.l   SR_set_IMASK, r1
    or  r3, r1
    mov.l   SR_clear_BL, r2
    and r2, r1
    ldc r1, sr

    /* Involve the kernel panic */
    mov.l   kernel_tlbh_panic, r0
    jmp @r0
    mov r15, r5


.balign 4

kernel_tlbh_panic:
    .long   _sh7305_intc_tlbh_panic
SR_set_IMASK:
    .long   (0xf << 4)
SR_clear_BL:
    .long   ~(1 << 28)
