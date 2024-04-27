/*
    Linker script for the fxcg50 platform.
*/
ENTRY(_initialize)

/*
** Linker script for user executables.
*/
MEMORY
{
    /* virtual memory, read-write segment */
    userram (wx)    : o = 0x00000000, l = 1M
    /* On-chip X memory */
    xram (rwx)      : o = 0xe5007000, l = 8k
}

SECTIONS
{
    /* set the base address to zero. This is really usefull when patching
     * symbols "on-the-fly" during runtime */
    . = 0x00000000 ;

    /* used for the self-patching step if needed */
    ___kernel_code_start = . ;

    /* explicitly isolate the "pre text" code which should be executed
     * first */
    .kernel : {
        KEEP(*(.kernel.header))
        KEEP(*(.kernel.pre_text))
        KEEP(*(.kernel.text))
    }

    /* Code */
    .text : {
        *(.text.entry);
        *(.text);
        *(.text.*);

        _bctors = . ;
        *(.ctors .ctors.*)
        _ectors = . ;

        _bdtors = . ;
        *(.dtors .dtors.*)
        _edtors = . ;

    }

    /* vhex's interrupt handler blocks (.vhex.blocks)
       Although vhex's blocks end up in VBR space, they are relocated at
       startup by the library/drivers, so we store them here for now */
    .vhex.blocks : {
        KEEP(*(SORT_BY_NAME(.vhex.blocks.*)));
    }

    /* Exposed driver interfaces (.vhex.drivers)
       The driver information is required to start and configure the
       driver, even if the symbols are not referenced */
    .vhex.drivers : {
        _vhex_drivers_start = . ;
        KEEP(*(SORT_BY_NAME(.vhex.drivers.*)));
        _vhex_drivers_end = . ;
    }

    /* Exposed module interfaces (.vhex.modules) */
    .vhex.modules : {
        _vhex_modules_start = . ;
        KEEP(*(SORT_BY_NAME(.vhex.modules.*)));
        _vhex_modules_end = . ;
    }

    /* Exposed device interfaces (.vhex.devices) */
    .vhex.devices : {
        _vhex_devices_start = . ;
        KEEP(*(SORT_BY_NAME(.vhex.devices.*)));
        _vhex_devices_end = . ;
    }

    /* Read-only sections */
    .rodata : {
        *(.rodata);
        *(.rodata.*);
    } > userram

    /* readable / writable data */
    .data ALIGN(4) : {
        *(.data);
        *(.data.*);
    } > userram

    /* Vhex VBR management geometry

       Due to the SH3/SH4 VBR system, we have some restrictions:

         The handlers should reside at VBR relative position, in P1 or P2
       protected space (0x8* or 0xA*). For our case, the bootloader will
       relocalise the kernel at the P1 area using the Casio's UTLB
       information. So, we don't have to wories about this here.

         There are 3 vectors offset called by the processor : VBR + 0x100,
       0x400 and 0x600. The first offset is involved when an exception
       occur, the second when a memory (TLB) error is detected and the last
       when an interruptions is detected.

         All exception and memory interuption will be handled "normaly" : a
       simple handler will be involved which will try to resolve/fix the
       problem, if nothing can be fixed, a panic screen will be displayed.

         However, regarding interuptions, we will use a power-full and
       complexe technique to compact as most as possible the VBR size. To
       explain the technique we need to know that each interruption start at
       offset VBR + 0x600 and each interrupt source is "gapped" by 0x20 (
       for exemple the 0x400 is for TMU0, 0x420 is for TMU1 and 0x440 is for
       TMU2) Moreover, we have some "hole" in the "interrup map".
       So, the idea is to write all interrupt handler in "blocks" of 32
       bytes, and, for those that 32 bytes is to short, can use hole of the
       interrupt map. */
    .vhex.vbr : ALIGN(4) {

        _vhex_vbr = . - 0x100;
        *(.vhex.exch.pretext) ;
        *(.vhex.exch)

        . = _vhex_vbr + 0x400;
        *(.vhex.tlbh.pretext) ;
        *(.vhex.tlbh) ;

        . = _vhex_vbr + 0x600;
        *(.vhex.inth.pretext) ;
        *(.vhex.inth) ;

    } > userram

    /* We should place the .bss section at the end of the "userram".
     *
     * This because it seems, after multiple test, that `ld` cannot handle
     * consistently the `.bss` section if it takes part of a single "segment"
     * without being in an edge of it.
     *
     *  +------------------+--------------+-----------+
     *  | <vxkernel stuff> |     .bss     | .vhex.vbr |  -> .bss is not wiped
     *  |------------------|-----------+--+-----------+
     *  | <vxkernel stuff> | .vhex.vbr |     .bss     |  -> .bss is wiped
     *  +--------------+---+-----------+--+-----------+
     *  |     .bss     | <vxkernel stuff> | .vhex.vbr |  -> .bss is wiped
     *  +--------------+------------------+-----------+
     *
     *  But in our case, we need to place the section at the end of the
     *  "userram" because we use the end of the `.bss` is used for malloc
     *  area.
     *  We also use the start of this section as a gap for interrupt gates
     *  mapped "on-the-fly" during the boot. This technique "de rat" allow
     *  us to save ~3k in the final ELF. */
    .bss ALIGN(4) : {

        /* set some space for interrupt gates
         *
         * - interruption event range is [0x400;0xfa0]
         * - interruption granularity is 0x20 (0x400, 0x420, 0x440, ...)
         *
         * So, the gap for each gates will be:
         *  (0xfa0 + 0x020) - 0x400 = 0xbc0,
         * which give us 3008 bytes. We will use 3ko. */
        . = . + 3072;

        /* bss section included to avoid missaligned segment */
        *(.bss);
        *(.bss.*);
        *(COMMON);

        /* dynamic BSS information (move me ?) */
        *(.dynbss)

        /* Video RAM symbols

           The Video RAM contains a full pixel sized frame for the
           screen. Its size is 396x224 and each pixel depth is 16bits,
           so (3996 * 224) * 2 = 177408

           Thus, we use a double buffering methods to speed-up the
           render, this is why we define two VRAM */

        _vhex_vram0 = ALIGN(32);
        . = _vhex_vram0 + 177408;
        _vhex_vram1 = ALIGN(32);
        . = _vhex_vram1 + 177408;

        /* static ram start (for kmalloc) */
        ___sram_start = ALIGN(16);

    } :NONE

    /*
    ** On-chip memory sections: X
    */

    . = ORIGIN(xram);
    .xram ALIGN(4) : ALIGN(4) {
        *(.vhex.xram)
        . = ALIGN(16);
    } > xram

    /*
    ** Unwanted section
    */

    /DISCARD/ : {
        *(.rela.debug*)
        *(.gnu.*)
        *(.debug_info)
        *(.debug_abbrev)
        *(.debug_loc)
        *(.debug_aranges)
        *(.debug_ranges)
        *(.debug_line)
        *(.debug_str)
        *(.debug_*)
        *(.jcr)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.comment)
        *(.interp)
    }
}
