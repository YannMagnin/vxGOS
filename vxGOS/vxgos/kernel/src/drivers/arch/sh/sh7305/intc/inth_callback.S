#define ASM_SOURCE
#include <vhex/hardware.h>
!#include "fxcg50/hardware.h"
.text

.global _sh7305_inth_callback

/* CALLBACK HELPER
   This function implements the callback with context saving. It is a general
   function and does not need to reside in VBR space which is block-structured.
   This function saves registers r0_bank...r7_bank, enables interrupts,
   switches back to user bank and executes the callback. It does not save other
   registers (pr/mach/macl/gbr) which are managed by the handler entry. */

/* sh7305_inth_callback: Indirect call from kernel space to userland
   @r4  Address of callback function
   -> Returns the return value of the callback (int). */
_sh7305_inth_callback:
    stc.l   r0_bank, @-r15
    stc.l   r1_bank, @-r15
    stc.l   r2_bank, @-r15
    stc.l   r3_bank, @-r15
    stc.l   r4_bank, @-r15
    stc.l   r5_bank, @-r15
    stc.l   r6_bank, @-r15
    stc.l   r7_bank, @-r15
    stc.l   spc, @-r15
    stc.l   ssr, @-r15
    stc.l   sr, @-r15

    /* Save some values to user bank; once we enable interrupts, the kernel
       bank might be overwritten at any moment. */
    ldc r4, r0_bank

    /* Enable interrupts and go back to user bank. On SH4, SR.IMASK is set
       to the level of the current interrupt, which makes sure we can only
       be re-interrupted by something with a higher priority. */
    stc sr, r1
    mov.l   .SR_clear_RB_BL, r0
    and r0, r1

    /* On the fx-CG emulator, it is outright ignored. In these situations,
       set IMASK to 15 to block interrupts while allowing TLB misses to be
       handled. */
    ! FIXME : board-specific
    !mov.l   .vhex, r2
    !mov.l   @(4*HWDEVICE,r2), r0
    !cmp/eq  #HWDEVICE_FXCG_MANAGER, r0
    !bt  .set_imask

    !bra .load_sr
    !nop

.set_imask:
    mov.l   .SR_set_IMASK, r0
    or  r0, r1

.load_sr:
    ldc r1, sr

    /* We are now in the user bank with r0 set. Perform the call. We want
       to forward the return value to kernel bank, but this bank can be
       changed at any moment since interrupts are enabled. */
    sts.l   pr, @-r15
    mov.l   @(4, r0), r4
    mov.l   @(8, r0), r5
    mov.l   @(12, r0), r6
    mov.l   @(16, r0), r7
    mov.l   @r0, r0
    jsr @r0
    nop
    lds.l   @r15+, pr

    /* Restore the previous status register and the registers of the
       interrupted procedure. Restoring sr gets us back to system bank with
       interrupts disabled. */
    ldc.l   @r15+, sr

    /* We can now pull the return value since interrupts are disabled */
    stc r0_bank, r0

    ldc.l   @r15+, ssr
    ldc.l   @r15+, spc
    ldc.l   @r15+, r7_bank
    ldc.l   @r15+, r6_bank
    ldc.l   @r15+, r5_bank
    ldc.l   @r15+, r4_bank
    ldc.l   @r15+, r3_bank
    ldc.l   @r15+, r2_bank
    ldc.l   @r15+, r1_bank
    rts
    ldc.l   @r15+, r0_bank

.align 4
.SR_clear_RB_BL:
    .long   ~((1 << 29) | (1 << 28))
.SR_set_IMASK:
    .long   (0xf << 4)
.vhex:
    .long   _vhex
