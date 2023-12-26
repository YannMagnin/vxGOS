/* _sh7305_intc_generic_handler_fate: Standard interrupt handler

   This is a generic interrupt handler that calls back into a C function,
   useful for complex handling or simple drivers that benefit more from
   simplicity than razor-sharp performance. */

.section .vhex.blocks, "ax"
.align 4

.global _sh7305_intc_inth_generic_gate /* 32 bytes */

_sh7305_intc_inth_generic_gate:
    mova    1f, r0
    mov.l   2f, r1
    jmp @r1
    mov r0, r4

1:  .zero   20  /* Indirect call to be made   (set at runtine) */
2:  .long   0   /* _sh7305_vhex_inth_callback (set at runtime) */
