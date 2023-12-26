.section    .bootloader.pre_text, "ax"
.global     __raspi3b_bootloader_start

.extern     __aarch64_image_hdr

__raspi3b_bootloader_start:

    // ---
    // *CRITICAL*
    //
    // The next four instructions will be patched *DURING COMPILE TIME* with
    // the complete image size (bootloader, ASLR, kernel, OS,...) as follows:
    //   > mov      x0, #<size0>
    //   > mov      x0, #<size1>, lsl 16
    //   > mov      x0, #<size2>, lsl 32
    //   > mov      x0, #<size3>, lsl 48
    // If not patched, the code will just wait a manual reset
    // ---

_patch_trap:
    wfe
    b   _patch_trap
    nop
    nop

translation_entry:

    // ---
    // prepare alias
    // ---
    #define image_size      x0
    #define ram_base_addr   x1
    #define rom_base_addr   x2

    // ---
    // handle secondary CPUs
    //
    // For now, each secondary CPUs will hang in a infinite wait loop. This
    // will be changed as soon as the bootloader is stable
    // ---

    mrs     x3, mpidr_el1
    and     x3, x3, #3
    cbz     x3, _primary_continue
_secondary_wait_loop:
    wfe
    b       _secondary_wait_loop
_primary_continue:

    // ---
    // Translate the image in RAM
    //
    // For now, no check is performed for the RAM size and all this part relies
    // on material assumption
    //
    // @note
    // - 64 = the size of the image.S file
    // - ram_base_addr is hardcoded here
    //
    // @todo
    // - check if we are already in RAM
    // - detect RAM geometry (size, start and end)
    // - ramdom translation offset
    // - request mailboxs hardware information
    // ---

    adr     x3, patch_aslr_symbols
    add     x3, x3, #(__raspi3b_bootloader_start - patch_aslr_symbols - 64)
    mov     rom_base_addr, x3
    mov     ram_base_addr, 0x00000000

    // FIXME : for now, assume that we will always be loaded in RAM
    mov     ram_base_addr, rom_base_addr

#if 0
    mov     x4, image_size
    mov     x5, rom_base_addr
    mov     x6, ram_base_addr
_ram_translation_loop:
    cbz     x4, _ram_translation_validate
    ldr     x7, [x5], #16
    str     x7, [x6], #16
    sub     x4, x4, #16
    cbnz    x4, _ram_translation_loop

_ram_translation_validate:
    wfe
    b   _ram_translation_validate
#endif

    // ---
    // Patch ASLR
    //
    // perform ASLR patch by using the symbols table information injected
    // during bootloader build steps at the end of the bootloader code marked
    // with ___bootloader_code_end.
    //
    // The content of the table has been generated during post-compiling script
    // ---

patch_aslr_symbols:
    // The table symbol is not aready resolved (its our job), we must manually
    // calculate the real address of the symbols table
    // @note
    // <> the symbol is defined by the linker script
    // <> this symbols don't need to perform ROM_BASE address subtraction
    //      since its content is based on the linker script provided address
    //      0x00000000000000000
    adr     x4, ___bootloader_code_end
_aslr_symbol_patch_loop:
    ldr     x5, [x4], #8
    cbz     x5, stack_setup
    add     x5, x5, ram_base_addr
    ldr     x6, [x5]
    add     x6, x6, ram_base_addr
    str     x6, [x5]
    b   _aslr_symbol_patch_loop

    // ---
    // RAM translation
    // ---

    // TODO  : relocation trampoline mecanism
    // TODO  : self-RAM translation
    // FIXME : invalidate cache
    // TODO  : erease trampoline mecanism
    // TODO  : jump to the relocalized RAM

    // ---
    // Setup stack
    // ---

stack_setup:
    adr     x4, __bootloader_stack
    mov     sp, x4

    // ---
    // High-level bootloader invokation
    // ---

high_level_bootloader_entry:
    mov x2, ram_base_addr
    mov x1, image_size
    mov x0, x2
    b   bootloader_fake_entry
    // TODO

    // ---
    // Bootloader panik if returned
    // ---

_bootloader_panik:
    wfe
    b       _bootloader_panik
