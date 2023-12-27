#include "fxcg50/asm_utils.h"

.section    .bootloader.text, "ax"

/* ___fxcg50_bootloader_start() : real bootstrap entry

Now we are in the URAM we can performs ASLR patching, setup stack and involve
the first high-level C code which will perform kernel setup.

The primary (fake) bootloader (previous operations) have setup some arg:
- r4 = ROM relocation base address
- r5 = RAM relocation base address (physical)
- r6 = image size */

function(__fxcg50_bootloader_start):

    ! ---
    ! prepare alias
    ! ---

    #define rom_reloc_base  r4
    #define ram_reloc_base  r5
    #define image_size      r6

    ! ---
    ! ASLR application
    !
    ! perform ASLR patch by using the symbols table information injected
    ! during bootloader build steps at the end of the bootloader code marked
    ! with ___bootloader_code_end.
    !
    ! The content of the table has been generated during post-compiling script
    ! ---

    ! The table symbol is not aready resolved (its our job), we must manually
    ! calculate the real address of the symbols table
    mov.l   bootloader_code_end, r0
    mov.l   p1_addr_base, r1
    mov.l   p2_addr_base, r2
    or      ram_reloc_base, r2
    or      ram_reloc_base, r1
    add     r2, r0

    ! walk trough the symbols table and patch all location
    ! @note
    ! - we MUST perform patching using P2 (uncachable) area to avoid
    !   inconsistancy behaviour with the cache.
    ! - symbols are relocalize through P1 (cachable) area
aslr_symbol_patch_loop:
    mov.l   @r0, r8
    tst     r8, r8
    bt      aslr_commit
    add     r2, r8
    mov.l   @r8, r9
    add     r1, r9
    mov.l   r9, @r8
    mov.l   r8, @r0
    add     #4, r0
    bra     aslr_symbol_patch_loop
    nop

aslr_commit:
    ! Now that ASLR symbols has been updated using uncachable area (P2), we
    ! need to invalitate all Operands Cache entry that the MPU have possibly
    ! setup to avoid inconsistant `mov.x` behaviour
    ! @note
    ! - CCR.OCI = 1  -> Operand Cache Invalidation (self-cleared to 0)
    mov.l   ccr_reg_addr, r1
    mov.l   @r1, r0
    or      #0x08, r0
    mov.l   r0, @r1
    synco

setup_stack:
    ! TODO : watermark stack area for statistics
    ! TODO : stack switch
    ! TODO : stack canary

bootloader_c_invokation:
    mov.l   p2_addr_base, r2
    mov     ram_reloc_base, r4
    mov     image_size, r5
    mov.l   bootloader_main, r0
    jsr     @r0
    or      r2, r4

    ! ---
    ! bootloader panic
    !
    ! As we have probably wierdly restored hadware information, if the
    ! bootloader main routine return we simply display black screen. You can
    ! uncomment following instruction to allows getkey() to return to the menu
    ! (debug only)
    ! ---

bootloader_paniktamer:
    mov.l   syscall_trampoline, r8
    mov     #0x4a, r4
    mov     #3, r5
    mov.l   syscall_id, r0
    jsr     @r8
    nop
test1:
!    add     #-4, r15
!    mov     r15, r4     ! column
!    add     #-4, r15
!    mov     r15, r5     ! row
!    add     #-4, r15
!    mov     r15, r1     ! keycode
!    mov     #0, r6      ! type of waiting (KEYWAIT_HALTON_TIMEOFF)
!    mov     #0, r7      ! timeout period
!    mov.l   r1, @-r15   ! keycode
!    mov     #0, r2
!    mov.l   r2, @-r15   ! [menu] key return to menu
!    mov.l   getkey_id, r0
!    jsr     @r8
!    nop
    bra     test1
    nop

.balign 4

bootloader_main:        .long _bootloader_main
bootloader_code_end:    .long ___bootloader_code_end
syscall_trampoline:     .long 0x80020070
syscall_id:             .long 0x0276
p2_addr_base:           .long 0xa0000000
p1_addr_base:           .long 0x80000000
getkey_id:              .long 0x000012bf    ! GetKeyWait_OS syscall ID
ccr_reg_addr:           .long 0xff00001c    ! SH7305.CACHE.CCR register address
