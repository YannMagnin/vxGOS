#if defined(__SUPPORT_FX9860__)
# include "fx9860/asm_utils.h"
#elif defined(__SUPPORT_FXCG50__)
# include "fxcg50/asm_utils.h"
#else
# error "unsupported device"
#endif

.section    .bootloader.text, "ax"

/* ___fxcg50_bootloader_start() : real bootstrap entry
 *
 * This routine is mutual between the FXC50 and FX98060, which have an exact
 * same starting runtime environment.
 *
 * Now that we are in the URAM we can perform ASLR patching, setup stack and
 * involve the first high-level C code that will perform bootloader setup.
 *
 * The primary (fake) bootloader (previous operations) have setup some arg:
 * - r0  (res) image ASLR table offset
 * - r1  (res) image size
 * - r2  (res) kernel entry offset
 * - r3  (res) kernel size
 * - r4  (res) virtualised base address
 * - r5  (res) URAM address
 * - r6  (res) URAM size */

function(__fxcg50_bootloader_start):

    ! ---
    ! ASLR application
    !
    ! perform ASLR patch by using the symbols table information injected
    ! during bootloader build steps
    ! ---

    ! The table symbol is not already resolved (it's our job), we must
    ! manually calculate its real address
    ! @register
    ! - r2  - (out) kernel entry address (P1)
    ! - r5  - (in) URAM address
    ! - r7  - (out) URAM address (P1)
    ! - r8  - tempo
    ! - r0  - (out) image symbols table address (P2)
    mov.l   data_80000000, r7
    mov.l   data_a0000000, r8
    or      r5, r7
    or      r5, r8
    or      r7, r5
    add     r8, r0
    add     r7, r2

    ! walk through the symbols table and patch all location
    ! @note
    ! - we MUST perform patching using P2 (uncacheable) area to avoid
    !   inconsistency behaviour with the cache.
    ! - symbols are relocalize through P1 (cacheable) area
    ! @register
    ! - r0  (in) image ASLR table address (P2)
    ! - r1  (res) image size
    ! - r2  (res) kernel entry offset
    ! - r3  (res) kernel size
    ! - r4  (res) virtualised base address
    ! - r5  (res) URAM address
    ! - r6  (res) URAM size
    ! - r7  (in) URAM physical base address (P1)
    ! - r8  (in) URAM physical base address (P2)
    ! - r9  /
    ! - r10 /
    ! - r11 /
aslr_symbol_patch_loop:
    mov.l   @r0, r10
    tst     r10, r10
    bt      aslr_commit
    add     r8, r10
    mov.b   @r10+, r11
    shll8   r11
    mov.b   @r10+, r9
    extu.b  r9, r9
    or      r9, r11
    shll8   r11
    mov.b   @r10+, r9
    extu.b  r9, r9
    or      r9, r11
    shll8   r11
    mov.b   @r10+, r9
    extu.b  r9, r9
    or      r9, r11
    add     r7, r11
    mov.b   r11, @-r10
    shlr8   r11
    mov.b   r11, @-r10
    shlr8   r11
    mov.b   r11, @-r10
    shlr8   r11
    mov.b   r11, @-r10
    add     #4, r0
    bra     aslr_symbol_patch_loop
    nop

aslr_commit:
    ! now that ASLR symbols has been updated using uncacheable area (P2), we
    ! need to invalidate all Operands Cache (OC) entry that the MPU have
    ! possibly setup to avoid inconsistent `mov.x` behaviour
    ! @note
    ! - CCR.OCI = 1  -> Operand Cache Invalidation (self-cleared to 0)
    mov.l   ccr_reg_addr, r10
    mov.l   @r10, r11
    mov     #0x08, r12
    or      r12, r11
    mov.l   r11, @r10
    synco

setup_stack:
    ! TODO : watermark stack area for statistics
    ! TODO : stack switch
    ! TODO : stack canary

bootloader_c_invokation:
    mov     r5, r4
    mov     r1, r5
    mov     r2, r6
    mov     r3, r7
    mov.l   bootloader_main, r0
    jsr     @r0
    nop

    ! ---
    ! bootloader panic
    ! ---

bootloader_panic:
    bra     bootloader_panic
    nop

.balign 4

bootloader_main:    .long _bootloader_main
data_a0000000:      .long 0xa0000000
data_80000000:      .long 0x80000000
ccr_reg_addr:       .long 0xff00001c
