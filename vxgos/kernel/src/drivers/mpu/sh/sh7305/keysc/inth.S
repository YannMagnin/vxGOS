/*
** keysc:inth.s - Interrupt handlers for the timer units
*/
.section    .vhex.blocks, "ax"

/* Gate for the standard Key Scan */
.global _sh7305_keysc_inth

! Notes:
! <> This gate use two block (64 bytes)
! <> Code -> 30 bytes
! <> Data -> 12 bytes

.align 4
_sh7305_keysc_inth:
    /* prologue (4 bytes) */
    sts.l   pr, @-r15
    mov.l   r8, @-r15

    /* Set BL=0, IMASK=15 (12 bytes) */
    stc sr, r8
    mov.l   sr_set_imask, r1
    or  r8, r1
    mov.l   sr_clear_bl, r2
    and r2, r1
    ldc r1, sr

    /* Try to involve the user-space handler (6 bytes) */
    mov.l   keysc_user_handler, r0
    jsr @r0
    nop

    /* restore SR  (2 bytes) */
    ldc r8, sr

    /* epilogue (8 bytes) */
    mov.l   @r15+, r8
    lds.l   @r15+, pr
    rts
    nop

.zero   20

sr_set_imask:       .long   (0xf << 4)
sr_clear_bl:        .long   ~(1 << 28)
keysc_user_handler: .long   _sh7305_keysc_int_handler
