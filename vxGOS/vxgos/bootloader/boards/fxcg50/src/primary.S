#include "fxcg50/asm_utils.h"

.section    .bootloader.pre_text, "ax"

/* ___fxcg50_initialize() : bootstrap routine

We are currently virtualized as a common program (addin) at 0x00300000, but the
code currently executed is physically fragmented through the ROM. So, we cannot
perform ASLR self-relocation here.

The first thing we need to do is find the "user" RAM allocated by Casio (by
analyzing the TLB) and then performs a "self-translation" into this place.
After that, we will be able to perform ASLR self-relocation and classic
"kernel" bootstrapping.. */

function(__fxcg50_primary_bootloader):

    ! ---
    ! *CRITICAL*
    !
    ! The next two instructions will be patched *DURING COMPILE TIME* with the
    ! complete image size (bootloader, ASLR, kernel, OS,...) as follows:
    !   > mov.l os_image_size, r0
    !   > mov.l kernel_image_offset, r1
    !   > mov.l kernel_image_size, r2
    ! If not patched, the code will just return and indicate to Casio not to
    ! restart the application.
    ! ---

    nop
    rts
    mov     #1, r0
    bra     translation_entry
    nop
    nop

.long   0x00000000  ! OS image size
.long   0x00000000  ! kernel image offset
.long   0x00000000  ! kernel image size

translation_entry:

    ! ---
    ! prepare alias
    !
    ! We don't need to setup the stack because we are loaded from CasioABS
    ! (Casio OS) as a simple program (addin) with a given stack, which we will
    ! not use even if we use "special" registers such as r8, r9, ... as we
    ! will be careful to never return from here.
    !
    ! @note
    ! - r0  is reused
    ! - r1  is reused
    ! - r2  is reused
    ! - r7  is reused
    ! - r14 is reused
    ! ---

    #define counter             r0
    #define utlb_addr_val       r1
    #define utlb_data_val       r2
    #define uram_phys_addr      r3
    #define rom_base_addr       r4
    #define rom_image_size      r5
    #define utlb_data_ptr       r6
    #define utlb_addr_ptr       r7
    #define uram_virt_ptr       r8
    #define uram_phys_size      r9
    #define utlb_valid_bit      r10
    #define utlb_data_ptr_saved r11
    #define utlb_magic_array    r12
    #define utlb_addr_ptr_saved r13
    #define utlb_data_ppn_mask  r14

    ! ---
    ! Save critical information
    !
    ! Calculate the runtime loaded address of the addin and save the
    ! compile-time image size information
    ! ---

    ! save image size
    mov     r0, rom_image_size

    ! precalculate runtime loaded address
    mova    utlb_fetch_uram_info, r0
    add     #(___fxcg50_primary_bootloader - utlb_fetch_uram_info), r0
    mov     r0, rom_base_addr

    ! ---
    ! Configure cache
    !
    ! This is a critical part because we will move to the URAM and perform
    ! self-patching symbols during our execution. So, we need to have a
    ! consistent cache behavior to help the MPU follow our instructions.
    !
    ! The SH7305 has a special register for that called Cache Control Register
    ! (CCR), and it seems tobe the same as SH7724 (except that no official POC
    ! or information can invalidate the instruction Cache (IC) without causing
    ! the machine to crash):
    !
    ! - Indicate that P1 area use the "Write-back" method
    ! - Indicate that P0/P3 areas use the "Write-through" method
    ! - Enable Operand Cache (OC)
    ! - Enable Instruction Cache (IC)
    ! - Invalidate all Operand Cache (OC) entries
    ! ---

    mov.l   ccr_register_addr, r0
    mov.l   ccr_register_data, r1
    mov.l   r1, @r0
    synco

    ! ---
    ! UTLB scan to find URAM information
    !
    ! As explained above, we first need to find the user RAM given by Casio for
    ! our application, as we know that is an unused and stable space for us.
    !
    ! We will scan each TLB entry to find the user's RAM. The memory is
    ! virtually mapped by Casio using the same historical virtual address:
    ! 0x08100000. Also, all the given RAM is entierly (?) mapped by the
    ! operating system. Thus, we walk through the TLB until we don't find the
    ! next memory fragment; this will allow us to find the size of the RAM
    ! (which varies between models and emulators).
    !
    ! We will also take advantage of the fact that Casio *MAP* the virtual
    ! address 0x00000000 (NULL) for no valid reason. So, if we find this
    ! mapping, we will invalidate it to be sure that a NULL manipulated pointer
    ! will cause a TLBmiss exception.
    !
    ! TODO : invalidate NULL page
    ! ---

utlb_fetch_uram_info:

    ! fetch external information
    mov.l   data_00000100, utlb_valid_bit
    mov.l   data_08100000, uram_virt_ptr
    mov.l   data_f6000000, utlb_addr_ptr
    mov.l   data_14100c0a, utlb_magic_array
    mov.l   data_1ffffc00, utlb_data_ppn_mask
    mov.l   data_f7000000, utlb_data_ptr

    ! prepare internal vars
    mov     #0,  counter
    mov     #-1, uram_phys_addr
    mov     #0,  uram_phys_size
    mov     utlb_data_ptr, utlb_data_ptr_saved
    mov     utlb_addr_ptr, utlb_addr_ptr_saved

utlb_walk_loop:
    ! validity check
    ! @note
    ! - check the validity bit for each UTLB data and address entry
    ! - both data and address entry have the same Valid bit position
    mov.l   @utlb_addr_ptr, utlb_addr_val
    tst     utlb_valid_bit, utlb_addr_val
    bt      utlb_walk_cond_check
    mov.l   @utlb_data_ptr, utlb_data_val
    tst     utlb_valid_bit, utlb_data_val
    bt.s    utlb_walk_cond_check

    ! check VPN validity
    ! @note
    ! - "UTLB Address Array" (p239) - Figure 7.14
    ! - we need to clear the first 10 bits of the fetched UTLB data to get the
    !   the "real" virtual address (eliminate ASID, Dirty and Valid bits)
    shlr8   utlb_addr_val
    shlr2   utlb_addr_val
    shll8   utlb_addr_val
    shll2   utlb_addr_val
    cmp/eq  uram_virt_ptr, utlb_addr_val
    bf.s    utlb_walk_cond_check

    ! fetch the page size
    ! @note
    ! - "Unified TLB (UTLB) Configuration"(p221)
    ! - page size is weird to calculate for many hardware reasons, and this code
    !   is the literal  translation of :
    !     > size = ((data->SZ1 << 1) | data->SZ2) << 3;
    !     > size = 1 << ((0x14100c0a >> size) & 0xff);
    mov     #-1, r1
    mov     utlb_data_val, r0
    tst     #128, r0
    mov     #-1, r7
    negc    r1,  r1
    tst     #16, r0
    add     r1,  r1
    negc    r7,  r7
    or      r7,  r1
    mov     utlb_magic_array,r7
    shll2   r1
    add     r1, r1
    neg     r1, r1
    shad    r1, r7
    extu.b  r7, r1
    mov     #1, r7
    shld    r1, r7

    ! update counter / information
    add     r7, uram_phys_size
    add     r7, uram_virt_ptr

    ! check if the URAM physical address is already set
    mov     uram_phys_addr,r0
    cmp/eq  #-1,r0
    bf      utlb_page_found_restart

    ! calculate the physical address of the page (URAM)
    ! @note
    ! - "UTLB Data Array"(p240) - Figure 7.15
    ! - to fetch the Physical Address, we just need to isolate the PPN
    and     utlb_data_ppn_mask, utlb_data_val
    shlr8   utlb_data_val
    shlr2   utlb_data_val
    mov     utlb_data_val, uram_phys_addr
    shll8   uram_phys_addr
    shll2   uram_phys_addr

utlb_page_found_restart:
    mov     r13, utlb_addr_ptr
    mov     r11, utlb_data_ptr
    bra     utlb_walk_loop
    mov     #0, counter

utlb_walk_cond_check:
    ! update internal counter
    ! @notes
    ! - only 64 UTLB entry
    ! - UTLB entry (for data and address) gap is 0x100
    mov.l   data_00000100, r1
    add     #1,  counter
    cmp/eq  #64, counter
    add     r1,  utlb_addr_ptr
    bf.s    utlb_walk_loop
    add     r1,  utlb_data_ptr

    ! ---
    ! Self-translation to URAM
    !
    ! Now that we have the user RAM entry address (uram_phys_addr) and its size
    ! (uram_phys_size), we can self-translate to this location using a dummy
    ! byte-per-byte copy.
    !
    ! Note that, for now, no random installation offset is performed since
    ! predicting uncertain behavior is complex to choose for now.
    ! ---

self_translation:
    ! fetch bootloader ROM geometry information
    mov     rom_base_addr,  r0
    mov     rom_image_size, r2

    ! generate uncachable P2 URAM address
    ! TODO
    ! - random offset
    ! - check oversize
    mov.l   data_a0000000,  r1
    or      uram_phys_addr, r1

    ! dump the complet image into URAM
self_dump_uram:
    mov.b   @r0, r14
    mov.b   r14, @r1
    dt      r2
    add     #1, r0
    add     #1, r1
    bf.s    self_dump_uram
    nop

    ! Prepare the self-translation by calculating the new PC position using a
    ! P1 address to allow caching to be performed.
    ! @note
    ! - ___fxcg50_bootloader_start is a global symbol compiled with NULL as the
    !   base address. So, we don't have to calculate the gap between the start
    !   of the ROM and the symbol.
    mov.l   data_80000000,  r1
    or      uram_phys_addr, r1
    mov.l   real_bootloader_start, r0
    add     r1, r0

    ! self-translation
    mov     rom_image_size, r6
    mov     uram_phys_addr, r5
    mov     rom_base_addr,  r4
    jmp     @r0
    nop

.balign 4

data_08100000:      .long 0x08100000    ! Casio addin load virtual address
data_f6000000:      .long 0xf6000000    ! SH7305 UTLB Address address
data_f7000000:      .long 0xf7000000    ! SH7305 UTLB Data addresss
data_14100c0a:      .long 0x14100c0a    ! Magic UTLB page size table
data_1ffffc00:      .long 0x1ffffc00    ! UTLB Address PPN mask
data_00000100:      .long 0x00000100    ! UTLB entry gap and UTLB validity bit
data_a0000000:      .long 0xa0000000    ! P2 base address
data_80000000:      .long 0x80000000    ! P1 base address
ccr_register_addr:  .long 0xff00001c    ! SH7305.CACHE.CCR register address
ccr_register_data:  .long 0x0000010f    ! CCR configuration
real_bootloader_start:
    .long   ___fxcg50_bootloader_start
