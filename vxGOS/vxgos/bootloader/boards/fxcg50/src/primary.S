#include "fxcg50/asm_utils.h"

.section    .bootloader.pre_text, "ax"

/* ___fxcg50_initialize() : bootstrap routine

We are currently virtualised as a common program (addin) at 0x00300000, but
the code currently executed is physically fragmented through the ROM.
So, we cannot perform ASLR self-relocation here.

The first thing we need to do is find the "user" RAM allocated by Casio (by
analyzing the TLB) and then performs a "self-translation" into this place.
After that, we will be able to perform ASLR self-relocation and classic
"kernel" bootstrapping.. */

function(__fxcg50_primary_bootloader):

    ! ---
    ! *CRITICAL*
    !
    ! The next few instructions will be patched *DURING COMPILE TIME* with
    ! the complete image size (bootloader, ASLR, kernel, OS,...) as follows:
    !
    !   > mov.l os_image_aslr_offset, r0
    !   > mov.l os_image_size, r1
    !   > mov.l kernel_image_offset, r2
    !   > mov.l kernel_image_size, r3
    !
    ! If not patched, the code will just return and indicate to Casio not to
    ! restart the application.
    ! ---

    nop
    nop
    rts
    mov     #1, r0
    bra     translation_entry
    nop

.long   0x00000000  ! OS symbols table offset
.long   0x00000000  ! OS image size
.long   0x00000000  ! kernel image offset
.long   0x00000000  ! kernel image size

translation_entry:

    ! ---
    ! Save critical information
    !
    ! Calculate the runtime loaded address of the addin and save the
    ! compile-time image size information
    ! ---

    #define image_aslr_table_offset r0
    #define image_size              r1
    #define kernel_image_offset     r2
    #define kernel_image_size       r3
    #define image_base_addr         r4

    ! precalculate runtime loaded address
    mov     image_aslr_table_offset, r5
    mova    utlb_fetch_uram_info, r0
    add     #(___fxcg50_primary_bootloader - utlb_fetch_uram_info), r0
    mov     r0, image_base_addr
    mov     r5, image_aslr_table_offset
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

    mov.l   ccr_register_addr, r5
    mov.l   ccr_register_data, r6
    mov.l   r6, @r5
    synco

utlb_fetch_uram_info:

    ! ---
    ! UTLB scan to find URAM information
    !
    ! As explained above, we first need to find the user RAM given by Casio
    ! for our application, as we know that is an unused and stable space
    ! for us.
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
    ! mapping, we will invalidate it to be sure that a NULL manipulated
    ! pointer will cause a TLBmiss exception.
    !
    ! (todo) : invalidate the NULL page
    !
    ! @registers
    ! - r0  reserved
    ! - r1  reserved
    ! - r2  reserved
    ! - r3  reserved
    ! - r4  reserved
    ! - r5  (out) URAM address
    ! - r6  (out) URAM size
    ! - r7  (tmp) counter
    ! - r8  (tmp) UTLB address array ptr
    ! - r9  (tmp) UTLB data array ptr
    ! - r10 (tmp) URAM virtual address
    ! - r11 (tmp) scratch
    ! - r12 (tmp) scratch
    ! - r13 (tmp) scratch
    ! - r14 (tmp) scratch
    ! - r15 reserved
    ! ---

    #define uram_phys_addr      r5
    #define uram_phys_size      r6
    #define counter             r7
    #define utlb_addr_array_ptr r8
    #define utlb_data_array_ptr r9
    #define uram_virt_addr      r10

    ! prepare loop
    mov     #0,  counter
    mov     #-1, uram_phys_addr
    mov     #0,  uram_phys_size
    mov.l   data_08100000, uram_virt_addr
    bra     utlb_page_found_restart
    nop

utlb_walk_loop:
    ! validity check
    ! @note
    ! - check the validity bit for each UTLB data and address entry
    ! - both data and address entry have the same Valid bit position
    ! - the valid bit mask is 0x00000100
    ! @register
    ! - r8      - UTLB address array ptr
    ! - r9      - UTLB data array ptr
    ! - r13     - tempo
    ! - r14     - tempo
    mov     #1, r13
    shll8   r13
    mov.l   @utlb_data_array_ptr, r14
    tst     r13, r14
    bt      utlb_entry_is_invalid
    mov.l   @utlb_addr_array_ptr, r14
    tst     r13, r14
    bt      utlb_entry_is_invalid

    ! check VPN validity
    ! @note
    ! - "UTLB Address Array" (p239) - Figure 7.14
    ! - we need to clear the first 10 bits of the fetched UTLB data to get
    !   the "real" virtual address (eliminate ASID, Dirty and Valid bits)
    ! - the URAM virtal address is always 0x08100000
    ! @register
    ! - r10     - current URAM virtual page address
    ! - r14     - current UTLB data bitmap
    shlr8   r14
    shlr2   r14
    shll8   r14
    shll2   r14
    cmp/eq  r14, uram_virt_addr
    bf      utlb_entry_is_invalid

    ! fetch the page size
    ! @note
    ! - "Unified TLB (UTLB) Configuration"(p221)
    ! - page size is weird to calculate for many hardware reasons, and this
    !   code is the literal C translation of :
    !     > size = ((data->SZ1 << 1) | data->SZ2) << 3;
    !     > size = 1 << ((0x14100c0a >> size) & 0xff);
    ! @register
    ! - r10     - (in) current URAM virtual page address
    ! - r11     - tempo && (out) physical page size
    ! - r12     - tempo
    ! - r13     - tempo
    ! - r14     - (in) current UTLB data bitmap
    mov     #-1,  r12
    mov     r14,  r13
    mov     #128, r11
    tst     r11,  r13
    negc    r12,  r12
    mov     #16,  r11
    tst     r11,  r13
    add     r12,  r12
    mov     #-1,  r11
    negc    r11,  r11
    or      r11,  r12
    mov.l   data_14100c0a, r11
    shll2   r12
    add     r12, r12
    neg     r12, r12
    shad    r12, r11
    extu.b  r11, r12
    mov     #1,  r11
    shld    r12, r11

    ! update the URAM physical size and also update the virtual address of
    ! the URAM. We will restart the research from index zero to find the
    ! potential next URAM virtual page
    ! @register
    ! - r6      - (out) URAM size
    ! - r10     - (out) URAM virtual page address
    ! - r11     - (in)  physical page size
    add     r11, uram_phys_size
    add     r11, uram_virt_addr

    ! calculate the URAM physical address if needed
    ! @note
    ! - "UTLB Data Array"(p240) - Figure 7.15
    ! - to fetch the Physical Address, we just need to isolate the PPN
    ! @register
    ! - r5      - (out) URAM physical address ptr
    ! - r12     - tempo
    ! - r13     - tempo
    ! - r14     - (out) calulate URAM physical address ptr
    mov     #-1, r12
    cmp/eq  r12, uram_phys_addr
    bf      utlb_page_found_restart
    mov.l   @utlb_data_array_ptr, r14
    mov.l   data_1ffffc00, r13
    and     r13, r14
    mov     r14, uram_phys_addr

utlb_page_found_restart:
    ! @register
    ! - r13     - tempo
    ! - r8      - UTLB address array ptr
    ! - r9      - UTLB data array ptr
    ! - r7      - counter
    mov.l   data_f6000000, r13
    mov     r13, utlb_addr_array_ptr
    mov.l   data_f7000000, r13
    mov     r13, utlb_data_array_ptr
    bra     utlb_walk_loop
    mov     #0, counter

utlb_entry_is_invalid:
    ! update internal counter
    ! @notes
    ! - only 64 UTLB entry
    ! - UTLB entry (for data and address) gap is 0x100
    mov.l   data_00000100, r11
    add     #1,  counter
    mov     #64, r12
    cmp/eq  r12, counter
    add     r11,  utlb_addr_array_ptr
    add     r11,  utlb_data_array_ptr
    bf      utlb_walk_loop

self_translation:

    ! ---
    ! Self-translation to URAM
    !
    ! Now that we have the user RAM entry address (uram_phys_addr) and its
    ! size (uram_phys_size), we can self-translate to this location using a
    ! dummy byte-per-byte copy.
    !
    ! Note that, for now, no random installation offset is performed since
    ! predicting uncertain behavior is complex to choose for now.
    !
    ! TODO
    ! - random offset
    ! - check oversize
    !
    ! @registers
    ! - r0  (res) image ASLR table offset
    ! - r1  (res) image size
    ! - r2  (res) kernel entry offset
    ! - r3  (res) kernel size
    ! - r4  (res) virtualised base address
    ! - r5  (res) URAM address
    ! - r6  (res) URAM size
    ! - r7  (tmp) scratch
    ! - r8  (tmp) scratch
    ! - r9  (tmp) scratch
    ! - r10 (tmp) scratch
    ! - r11 (tmp) scratch
    ! - r12 (tmp) scratch
    ! - r13 (tmp) scratch
    ! - r14 (tmp) scratch
    ! - r15 reserved
    ! ---

    ! generate uncachable P2 URAM address
    mov.l   data_a0000000,  r7
    or      uram_phys_addr, r7
    mov     image_size, r8
    mov     image_base_addr, r9

    ! dump the complet image into URAM
self_dump_uram:
    mov.b   @r9, r14
    mov.b   r14, @r7
    dt      r8
    add     #1, r9
    add     #1, r7
    bf    self_dump_uram

self_jump:

    ! Prepare the self-translation by calculating the new PC position using
    ! the guessed physical URAM address and P1 mask to allow caching to be
    ! performed.
    ! @note
    ! - Since we have linked our OS image using the address 0x00000000, we
    ! don't have to calculate the gap between the start of the ROM and the
    ! symbol. Simply add them
    mov.l   data_80000000,  r11
    or      uram_phys_addr, r11
    mov.l   real_bootloader_start, r12
    add     r12, r11

    ! Now that we have uploaded all of the image in the URAM using
    ! uncachable area (P2), we need to invalitate all Operands Cache entries
    ! that the MPU have possibly setup to avoid inconsistant `mov.x`/`jmp`
    ! behaviour
    ! @note
    ! - CCR.OCI = 1  -> Operand Cache Invalidation (self-cleared to 0)
    mov.l   ccr_register_addr, r12
    mov.l   @r12,  r13
    mov     #0x08, r14
    or      r14,   r13
    mov.l   r13,   @r12
    synco

    ! self-translation
    !
    ! @register
    ! - r0  (res) image ASLR table offset
    ! - r1  (res) image size
    ! - r2  (res) kernel entry offset
    ! - r3  (res) kernel size
    ! - r4  (res) virtualised base address
    ! - r5  (res) URAM address
    ! - r6  (res) URAM size
    jmp     @r11
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
