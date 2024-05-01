/*
 * Generic bootloader linker script
 */

SECTIONS
{
    /* set the base address to zero. This is really useful when patching
     * symbols "on-the-fly" during runtime */
    . = 0x00000000 ;

    /* explicitly isolate the "pre text" code which should be executed
     * first */
    .bootloader : {
        KEEP(*(.bootloader.header))
        KEEP(*(.bootloader.pre_text))
        KEEP(*(.bootloader.text))
    }

    /* assembly code should be inserted here */
    .text : {
        *(.text.entry)
        *(.text)
        *(.text.*)
    }

    /* read-only sections */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    /* registered bootloader commands */
    .bootloader_cmd ALIGN(16) : {
        ___bootloader_cmd_array_start = . ;
        *(.bootloader_cmd)
        ___bootloader_cmd_array_end = . ;
    }

    /* uninitialized globals (must be 16-aligned)
     *
     * @note
     * The `.bss` section is annihilated by `objcopy` only if placed before
     * or after all other sections. However, if this section disappears, then
     * the kernel blob will be placed right after the last section, which
     * would create a collision between bootloader (uninitialized) globals
     * and kernel bootstrap. Therefore, ensure that this section is enclosed
     * within another section to avoid the collision with the kernel. */
    .bss ALIGN(16) : {
        *(.bss)
        *(.bss.*)
        . = ALIGN(16) ;
    }

    /* readable / writable data (must be 16-aligned) */
    .data ALIGN(16) : {
        *(.data)
        *(.data.*)
        *(COMMON)
        . = ALIGN(16) ;
    }

    /* We do not want any PLT/GOT information in this part of the code */
    .plt     : { *(.plt)     }
    .plt.got : { *(.plt.got) }
    ASSERT(SIZEOF(.plt) == 0, "unexpected '.plt' entries")
    ASSERT(SIZEOF(.plt.got) == 0, "unexpected '.plt.got' entries")

    /* gloabal offset table
     * We don't want/need any GOT entries, but BFD LD insert an entry
     * ("_DYNAMIC") into '.got', as required by ABI, and three empty entry on
     * some architecture.
     *
     * (fixme) : it seems that the `--emit-reloc` inject more symbols */
    .got : { *(.got.plt) *(.got) }
    /*ASSERT(
        SIZEOF(.got) <= (4 * 8),
        "'.got' should contain only ABI-required entries"
    )*/

    /* unwanted sections */
    /DISCARD/ : {
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
        *(.note.*)
    }
}
