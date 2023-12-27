/*
**  intc:inth.S - Interrupt handlers
**  This file only contains the entry points because the gates are managed
**  by device drivers. Each driver will supply its own interrupt handler
**  blocks depending on its configuration.
*/
.section    .vhex.inth.pretext, "ax"
.global _vhex_inth_sh7305

/* Interrupt handlers

   The .vhex.blocks section consists of blocks of 32 bytes intended for mapping
   into the VBR space (exception, TLB miss, and interrupt handlers). Each event
   gate is linearly associated with a block number on SH4:

     block_id = (event_code - 0x3c0) / 0x20

   This file provides entry points; drivers may provide their own interrupt
   handlers, and store them in the .vhex.blocks section for consistency. They
   should be aware of the consequences of reordering the blocks into the VBR
   space:

   - It is possible to map MPU-specific blocks at runtime, to avoid checking
     the MPU each time an interrupt is handled;
   - However, the blocks can only rely on relative displacements or cross-
     references if their relative order is known and heeded for both on SH4 and
     SH3. A good example of this is the timer driver. Please be careful. */


/* SH7305-TYPE INTERRUPT HANDLER ENTRY - 40 BYTES */

_vhex_inth_sh7305:
    /* Save caller-saved registers which might currently be in use by the
       interrupted function, as we don't know what the callback will do */
    sts.l   pr, @-r15
    stc.l   gbr, @-r15
    sts.l   mach, @-r15
    sts.l   macl, @-r15

    /* Get the event code from the INTEVT register */
    mov.l   1f, r0
    mov.l   @r0, r0

    /* Interrupt codes start at 0x400 */
    mov #4, r1
    shll8   r1
    sub r1, r0

    /* Add the distance to the first entry and jump as a subroutine */
    add #(.first_entry - .jump_over), r0
    bsrf    r0
    nop

.jump_over:
    /* Restore caller-saved registers */
    lds.l   @r15+, macl
    lds.l   @r15+, mach
    ldc.l   @r15+, gbr
    lds.l   @r15+, pr

    rte
    nop

    .zero   24
1:  .long   0xff000028
.first_entry:

