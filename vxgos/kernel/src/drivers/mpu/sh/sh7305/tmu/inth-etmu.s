/*
** vhex:tmu:inth-etmu - Interrupt handlers for the RTC-bound timers
**
** This handler uses 3 consecutive blocks like the TMU handler. However this
** time 2 empty blocks after ETMU4 (0xd20, 0xd40) are used because blocks for
** ETMU are not consecutive in memory.
**
** It would be possible to communicate between any interrupt handlers in non-
** consecutive gates. A simple way is to store at runtime a pointer to the
** desired object in one handler. But that costs a lot of space. If the
** relative position of interrupt handlers is known, the best option left is
** the unnatural @(disp,pc) addressing mode, and it doesn't even work with the
** SH3's compact VBR scheme.
*/

/* Gates for the extra timers (informally called ETMU) */
.global _sh7305_inth_etmu4 /* 96 bytes */
.global _sh7305_inth_etmux /* 32 bytes */

.section .vhex.blocks, "ax"
.balign 4

/* 3-block handler installed at the ETMU4 gate. */
_sh7305_inth_etmu4:
    mova    .storage_etmu4, r0
    mov     #7, r2

.shared:
    mov.l   r8, @-r15
    sts.l   pr, @-r15

    /* Prepare an indirect call to timer_stop(<id>) */
    add     #-20, r15
    mov.l   r2, @(4, r15)

    /* Clear interrupt flag in TCR using slow write */
    mov     r0, r1
    mov.l   @(4, r1), r3
TCR_clear:
    mov.b   @r3, r0
    tst     #0x02, r0
    and     #0xfd, r0
    bf/s    TCR_clear
    mov.b   r0, @r3

    /* Invoke callback */
    mov.l   sh7305_inth_callback, r8
    jsr     @r8
    mov.l   @r1, r4
    tst     r0, r0
    bt  ETMUx_shared_exit

    /* If return value is non-zero, stop the timer with another callback */
    mov.l   timer_stop, r0
    mov.l   r0, @r15
    jsr     @r8
    mov     r15, r4

ETMUx_shared_exit:
    add     #20, r15
    lds.l   @r15+, pr
    rts
    mov.l   @r15+, r8

    .zero   28

timer_stop:
    .long   _sh7305_tmu_stop
sh7305_inth_callback:
    .long   _sh7305_inth_callback
.storage_etmu4:
    .long   _sh7305_tmu_callbacks + 140
    .long   0xa44d00bc /* RTCR4 */


!---
! Generic gate for all other ETMU handlers, falling back to ETMU4.
!---
! Note
! <> all information needed in the storage_etmux is set on-the-fly when the
!    tmu handler is installed (see <driver/mpu/sh/sh7305/tmu/tmu.c>)
! <> the VBR computation is used to determine the address of the ETMU4 "real"
!    address during runtime
! <> the timer ID is set during the installation of the handler
_sh7305_inth_etmux:
    mov.l   etmu4_inth_addr, r3
    mov.w   etmu4_inth_soff, r2
    add     r2, r3
    nop

    mova    storage_etmux, r0
    mov.w   etmux_id, r2
    jmp     @r3
    nop

etmux_id:
    .word   0x0000                          ! Timer ID */

etmu4_inth_soff:
    .word   (.shared - _sh7305_inth_etmu4)  ! ETMU4 shared code off

etmu4_inth_addr:
    .long   0x00000000                      ! ETMU4 shared code addr

storage_etmux:
    .long   0x00000000                      ! _sh7305_tmu_callbacks
    .long   0x00000000                      ! TCR address
