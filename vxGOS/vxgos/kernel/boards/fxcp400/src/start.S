#include "fxcp400/asm_utils.h"

.section    .kernel.pre_text, "ax"

/* ___fxcp400_kernel_start() : kernel bootstrap entry

We are invoked through the vxGOS bootloader, which mean that we assumes that
we are in P1 area and we have no weird translation to perform (for example,
TLB analysis on fxcg50). We simply need to:

    - perform kernel symbols relocation
    - setup stack (canary)
    - invoke high-level (board-specific) kernel entry

Also note that some parameter will be given by the bootloader:
    - r4 : kernel image base physical address
    - r5 : kernel image size
*/
function(__fxcp400_kernel_start):

    !---
    ! prepare aliases
    !---

    #define kernel_image_base_addr  r4
    #define kernel_image_size       r5

    !---
    ! Critical section
    !
    ! next few instructions will be patched during post-compile time to
    ! register the kernel symbols table offset that is used for the
    ! self-relocation operation.
    !
    ! > mov.l   @(x,PC), r0
    ! > nop
    !---

    trapa   #0
    nop
    nop
    nop
    bra     cache_configuration
    nop

.long   0x00000000

cache_configuration:

    !---
    ! Configure the cache
    !
    ! This part is critical, because in the next step, we will patch all
    ! kernel symbols during our execution. So, we need to have a consistent
    ! cache behaviour to avoid jumping in a "seems patched" address that has
    ! not been written back by the physical cache.
    !
    ! So, as bootloader do, we will (re-)configure the Cache Control Register
    ! (CCR), and indicating we want:
    !
    ! - the "P1" area use the "Write-back" method
    ! - the "P0" and "P3" areas use the "Write-trough" method
    ! - enable the Operand Cache (OC)
    ! - enable the Instruction Cache (IC)
    ! - invalidate all Operand Cache entries (OC)
    !---

    mov.l   ccr_register_addr, r1
    mov.l   ccr_register_data, r2
    mov.l   r2, @r1
    synco

kernel_symbols_patching:

    ! ---
    ! kernel symbols patching
    !
    ! perform kernel symbols patching by using the symbols table information
    ! (injected during the image build steps) at the end of the kernel code
    ! marked with ___kernel_code_end.
    ! ---

    ! The table symbol is not already resolved (its our job), we must
    ! manually calculate the real address of the symbols table
    mov.l   p1_addr_base, r1
    mov.l   p2_addr_base, r2
    or      kernel_image_base_addr, r2
    or      kernel_image_base_addr, r1
    add     r2, r0

    ! walk trough the symbols table and patch all location
    ! @note
    ! - we MUST perform patching using P2 (uncachable) area to avoid
    !   inconsistency behaviour with the physical cache.
    ! - symbols are relocalized through P1 (cachable) area
_kernel_symbols_patch_loop:
    mov.l   @r0, r8
    tst     r8, r8
    bt      _kernel_symbols_patch_commit
    add     r2, r8
    mov.l   @r8, r9
    add     r1, r9
    mov.l   r9, @r8
    mov.l   r8, @r0
    add     #4, r0
    bra     _kernel_symbols_patch_loop
    nop

    mov     #5, r0
    rts
    nop

_kernel_symbols_patch_commit:
    ! Now that kernel symbols has been updated using uncachable area (P2), we
    ! need to invalidate all Operands Cache (OC) entries that the MPU have
    ! possibly setup to avoid inconsistent `mov.x` behaviour
    ! @note
    ! - CCR.OCI = 1  -> Operand Cache Invalidation (self-cleared to 0)
    mov.l   ccr_register_addr, r1
    mov.l   @r1, r0
    or      #0x08, r0
    mov.l   r0, @r1
    synco

setup_stack:
    ! TODO : watermark stack area for statistics
    ! TODO : stack switch
    ! TODO : stack canary

kernel_entry_invokation:
    mov.l   kernel_entry, r0
    jsr     @r0
    nop

    ! ---
    ! bootloader panic
    !
    ! As we have probably weirdly restored hardware information, if the
    ! kernel initialisation routine return we simply wait a manual reset.
    !
    ! (todo) : red screen
    ! ---

_kernel_bootstrap_panic:
    sleep
    bra     _kernel_bootstrap_panic
    nop

! ---
! data part
! ---

.balign 4

kernel_entry:       .long _kernel_initialize
p2_addr_base:       .long 0xa0000000
p1_addr_base:       .long 0x80000000
ccr_register_addr:  .long 0xff00001c
ccr_register_data:  .long 0x0000010f
