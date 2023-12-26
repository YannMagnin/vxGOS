.text
.global     ___stack_chk_guard
.global     ___stack_chk_fail


.balign 4

___stack_chk_guard:
    .long   0xdeb0cad0

.balign 4

___stack_chk_fail:
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
    mov.l   r7, @-r15
    mov.l   r6, @-r15
    mov.l   r5, @-r15
    mov.l   r4, @-r15
    mov.l   r3, @-r15
    mov.l   r2, @-r15
    mov.l   r1, @-r15
    mov.l   r0, @-r15

    ! invoke the panic handler, it SHOULD never return
    mov.l   sh7305_cpu_stack_chk_fail, r0
    jsr     @r0
    mov     r15, r4

    ! handle panic handler error
panic_handler_error:
    bra     panic_handler_error
    nop


.balign 4

sh7305_cpu_stack_chk_fail:
    .long   _sh7305_cpu_stack_chk_fail
