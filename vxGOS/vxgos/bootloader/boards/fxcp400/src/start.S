#include "fxcp400/asm_utils.h"

.section    .bootloader.text, "ax"

/* ___fxcp400_bootloader_start() : real bootstrap entry

Now we are in the last 128ko of the physical RAM we can performs ASLR
patching, setup stack and involve the first high-level C code which will
perform kernel setup. */

function(__fxcp400_bootloader_start):

    ! ---
    ! *CRITICAL*
    !
    ! The next few instructions will be patched *DURING COMPILE TIME* with
    ! the complete image size (bootloader, ASLR, kernel, OS,...) as follows:
    !   > mov.l os_image_aslr_offset, r0
    !   > mov.l os_image_size, r1
    !   > mov.l kernel_image_offset, r2
    !   > mov.l kernel_image_size, r3
    ! If not patched, the code will just return and indicate to the CFW to
    ! not restart the application.
    ! ---

    nop
    mov     #1, r0
    rts
    nop
    bra     translation_entry
    nop

.long   0x00000000  ! OS image size
.long   0x00000000  ! OS symbols table offset
.long   0x00000000  ! kernel image offset in the image
.long   0x00000000  ! kernel image size

translation_entry:

    ! ---
    ! prepare alias
    ! ---

    #define image_aslr_table_offset r1
    #define image_reloc_base_addr   r4
    #define image_size              r5
    #define kernel_image_addr       r6
    #define kernel_image_size       r7

    ! ---
    ! Save critical information
    !
    ! Calculate the runtime loaded address of the addin and save the
    ! compile-time image size information
    ! ---

    ! save patched information
    mov     r1, image_size
    mov     r2, kernel_image_addr
    mov     r3, kernel_image_size
    mov     r0, image_aslr_table_offset

    ! precalculate runtime loaded address
    ! @note
    ! > we are currently in RAM (P1), we need to clear the 3 uppers bits of
    ! the guessed address to determine the physical
    mova    cache_configuration, r0
    add     #(___fxcp400_bootloader_start - cache_configuration), r0
    mov     r0, image_reloc_base_addr
    shll2   image_reloc_base_addr
    shll    image_reloc_base_addr
    shlr    image_reloc_base_addr
    shlr2   image_reloc_base_addr
    nop

cache_configuration:

    ! ---
    ! Configure cache
    !
    ! This is a critical part because we will move to the URAM and perform
    ! self-patching symbols during our execution. So, we need to have a
    ! consistent cache behavior to help the MPU follow our instructions.
    !
    ! The SH7305 has a special register for that called Cache Control
    ! Register (CCR), and it seems tobe the same as SH7724 (except that no
    ! official POC or information can invalidate the instruction Cache (IC)
    ! without causing the machine to crash):
    !
    ! - Indicate that P1 area use the "Write-back" method
    ! - Indicate that P0/P3 areas use the "Write-through" method
    ! - Enable Operand Cache (OC)
    ! - Enable Instruction Cache (IC)
    ! - Invalidate all Operand Cache (OC) entries
    ! ---

    mov.l   ccr_register_addr, r2
    mov.l   ccr_register_data, r3
    mov.l   r3, @r2
    synco

aslr_patching:

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

    ! The table symbol is not already resolved (its our job), we must
    ! manually calculate the real address of the symbols table
    mov     image_aslr_table_offset, r0
    mov.l   p1_addr_base, r1
    mov.l   p2_addr_base, r2
    or      image_reloc_base_addr, r2
    or      image_reloc_base_addr, r1
    add     r2, r0

    ! walk trough the symbols table and patch all location
    ! @note
    ! - we MUST perform patching using P2 (uncachable) area to avoid
    !   inconsistancy behaviour with the physical cache.
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
    mov.l   ccr_register_addr, r1
    mov.l   @r1, r0
    or      #0x08, r0
    mov.l   r0, @r1
    synco

setup_stack:
    ! TODO : watermark stack area for statistics
    ! TODO : stack switch
    ! TODO : stack canary

bootloader_c_invokation:
    mov     image_reloc_base_addr, r4
    mov     image_size, r5
    mov     kernel_image_addr, r6
    mov     kernel_image_size, r7
    mov.l   bootloader_main, r0
    jsr     @r0
    add     r4, r6

    ! ---
    ! bootloader panic
    !
    ! As we have probably wierdly restored hadware information, if the
    ! bootloader main routine return we simply display black screen. You can
    ! uncomment following instruction to allows getkey() to return to the
    ! menu (debug only)
    ! ---

bootloader_panic:
    sleep
    bra     bootloader_panic
    nop

! ---
! data part
! ---

.balign 4

bootloader_main:        .long _bootloader_main
p2_addr_base:           .long 0xa0000000
p1_addr_base:           .long 0x80000000
ccr_register_addr:      .long 0xff00001c
ccr_register_data:      .long 0x0000010f
