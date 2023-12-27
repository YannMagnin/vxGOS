.section    .bootloader.header, "ax"
.global     __aarch64_image_hdr

/* __aarch64_image_hdr - fake linux boot header

   This structure is placed at the first start of the final image binary have
   can have multiple role depending on the host which the project is executed,
   but generaly it's used to indicate how the image must be loaded in physical
   memory

   Also note that is the raspi find this header, will be loaded a 0x20000000
   instead of the classical 0x80000 which will allows us to performs the self
  translation as we want */
__aarch64_image_hdr:

    // This is a harmless instruction that also happens to have "MZ" as its
    // first two (little-endian) bytes, which is the EFI (PE-COFF) magic
    // number. Some boot loaders and file utility look for this, apparently.
    // @note
    // * instruction = if (!N) { x18 -= 0 } else { N:Z:C:V = 0x0d }
    // * pl = Conditional Code = Plus, Positive or zero = !N
    ccmp x18, #0x0, #0xd, pl

    // branch to the bootloader bootstrap entry
    b __raspi3b_bootloader_start

    // Image load offset
    .quad   0x0000000000000000

    // Image size.
    // This information is patched *DURING COMPILE TIME*
    .quad   0x0000000000000000

    // Loading Flags.
    // @note
    //  * [0b***0] select little-endian
    //  * [0b*01*] select kernel page size of 4ko
    //  * [0b1***] select only 2Mio-aligned loading address but random position
    .quad   0b1010

    // Reserved and magic information
    .quad   0x0000000000000000
    .quad   0x0000000000000000
    .quad   0x0000000000000000
    .ascii  "ARM\x64"
    .int    0x00000000
