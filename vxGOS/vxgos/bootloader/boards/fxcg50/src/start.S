#include "fxcg50/asm_utils.h"

.section    .bootloader.text, "ax"

/* ___fxcg50_bootloader_start() : real bootstrap entry

Now we are in the URAM we can performs ASLR patching, setup stack and involve
the first high-level C code which will perform kernel setup.

The primary (fake) bootloader (previous operations) have setup some arg:
- r0  (res) image ASLR table offset
- r1  (res) image size
- r2  (res) kernel entry offset
- r3  (res) kernel size
- r4  (res) virtualised base address
- r5  (res) URAM address
- r6  (res) URAM size
*/

function(__fxcg50_bootloader_start):

    ! ---
    ! ASLR application
    !
    ! perform ASLR patch by using the symbols table information injected
    ! during bootloader build steps at the end of the bootloader code marked
    ! with ___bootloader_code_end.
    !
    ! The content of the table has been generated during post-compiling
    ! script.
    ! ---

    ! The table symbol is not aready resolved (its our job), we must manually
    ! calculate the real address of the symbols table
    ! @register
    ! - r5  - (in) URAM address
    ! - r7  - (out) URAM address (P1)
    ! - r8  - tempo
    ! - r0  - (out) image symbols table address (P2)
    mov.l   data_80000000, r7
    mov.l   data_a0000000, r8
    or      r5, r7
    or      r5, r8
    add     r8, r0

    ! walk trough the symbols table and patch all location
    ! @note
    ! - we MUST perform patching using P2 (uncachable) area to avoid
    !   inconsistancy behaviour with the cache.
    ! - symbols are relocalize through P1 (cachable) area
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
    ! - r9  reserved
    ! - r10 reserved
    ! - r11 reserved
aslr_symbol_patch_loop:
    mov.l   @r0, r10
    tst     r10, r10
    bt      aslr_commit
    add     r8, r10
    mov.l   @r10, r11
    add     r7, r11
    mov.l   r11, @r10
    mov.l   r10, @r0
    add     #4, r0
    bra     aslr_symbol_patch_loop
    nop

aslr_commit:
    ! Now that ASLR symbols has been updated using uncachable area (P2), we
    ! need to invalitate all Operands Cache entry that the MPU have possibly
    ! setup to avoid inconsistant `mov.x` behaviour
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
    add     r4, r6

    ! ---
    ! bootloader panic
    !
    ! As we have probably wierdly restored hadware information, if the
    ! bootloader main routine return we simply display black screen. You can
    ! uncomment following instruction to allows getkey() to return to the
    ! menu
    ! ---

bootloader_panic:
    bra     bootloader_panic
    nop

.balign 4

bootloader_main:    .long _bootloader_main
data_a0000000:      .long 0xa0000000
data_80000000:      .long 0x80000000
ccr_reg_addr:       .long 0xff00001c
