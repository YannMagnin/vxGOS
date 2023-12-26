"""
generate - post-build script used to generate bootlaoder blob with ASLR
"""
import os
import sys
import subprocess

__all__ = [
    'generate_aslr_blob',
    'generate_image'
]

#---
# Internals
#---

def _patch_got_section(symfile, section):
    """ fetch all address of the GOT table
    """
    for sec in section:
        if sec[0] != '.got':
            continue
        got_base_addr = int(sec[2], base=16)
        for i in range(0, int(sec[4], base=16), 8):
            print(f"  > reloc GOT entry {got_base_addr+i:016x}...")
            symfile.write((got_base_addr + i).to_bytes(8, 'little'))
        break

#---
# Public
#---

def generate_aslr_blob(binpath, symtab, sectab):
    """ generate bootloader final blob with ASLR

    The objectif of this script is to generate the final bootloader blob with
    ASLR. To performs this, we will performs 3 steps:

        * generate the raw binary file of the bootloader (objcpy)
        * generate the raw ALSR symbols table
        * generate the complet blootloader image

    @args
    > binpath  (str) - binary file
    > symtab  (list) - list of all reloc information (readelf -r binpath)
    > sectab  (list) - list of all sections information (readelf -S binpath)

    @return
    > Nothings
    """
    print('- generate raw binary...')
    if os.path.exists(f"{binpath}.raw"):
        os.remove(f"{binpath}.raw")
    cmd = f"aarch64-linux-gnu-objcopy -O binary -R .bss {binpath} {binpath}.raw"
    subprocess.run(
        cmd.split(),
        capture_output=False,
        check=False
    )

    print('- generate raw symtab...')
    if os.path.exists(f"{binpath}.symtab"):
        os.remove(f"{binpath}.symtab")
    need_patch_got = False
    with open(f"{binpath}.symtab", 'xb') as symfile:
        for i, sym in enumerate(symtab):
            # direct 64bits address
            if sym[2] == 'R_AARCH64_ABS64':
                print(f"  > reloc 'R_AARCH64_ABS64' sym at {sym[0]}")
                symfile.write(int(sym[0], base=16).to_bytes(8, 'little'))
                continue

            # need GOT patch
            if sym[2] == 'R_AARCH64_ADR_GOT':
                need_patch_got = True
                continue
            if sym[2] == 'R_AARCH64_LD64_GO':
                continue

            # PC-related TAG
            if sym[2] in [
                'R_AARCH64_JUMP26',
                'R_AARCH64_CALL26',
                'R_AARCH64_ADR_PRE',
                'R_AARCH64_ADD_ABS',
                'R_AARCH64_LDST32_',
            ]:
                continue

            # unsupported TAG, aboard
            print(f"  > [{i}] reloc {sym[2]} not supported")
            sys.exit(84)

        # patch GOT if needed
        if need_patch_got:
            _patch_got_section(symfile, sectab)
        symfile.write(int('0000000000000000', base=16).to_bytes(8, 'little'))

    print('- generate the full bootloader...')
    if os.path.exists(f"{binpath}.bzImage"):
        os.remove(f"{binpath}.bzImage")
    image_size = 0
    with open(f"{binpath}.bzImage", 'xb') as bzimgfile:
        with open(f"{binpath}.raw", 'rb') as rawbinfile:
            content = rawbinfile.read()
            bzimgfile.write(content)
            image_size += len(content)
        with open(f"{binpath}.symtab", 'rb') as symtabfile:
            content = symtabfile.read()
            bzimgfile.write(content)
            image_size += len(content)
        if image_size % 512:
            for _ in range(0, 512 - (image_size % 512)):
                bzimgfile.write(0x00.to_bytes(1))
    return f"{binpath}.bzImage"


def generate_image(prefix_build, bootloader_path, _):
    """ generate complet image file
    """
    # mov x0, #0x0000         = 0xd2800000  -  0xd29fffe0
    # mov x0, #0x0000, lsl 16 = 0xf2a00000  -  0xf2bfffe0
    # mov x0, #0x0000, lsl 32 = 0xf2c00000  -  0xf2dfffe0
    # mov x0, #0x0000, lsl 48 = 0xf2e00000  -  0xf2ffffe0

    image = bytearray(0)
    with open(bootloader_path, 'rb') as bootloaderfile:
        image += bootloaderfile.read()
    # (todo) os/kernel
    image_size = len(image)

    mov0 = 0xd2800000 | (((image_size & 0x000000000000ffff) >>  0) << 5)
    mov1 = 0xf2a00000 | (((image_size & 0x00000000ffff0000) >> 16) << 5)
    mov2 = 0xf2c00000 | (((image_size & 0x0000ffff00000000) >> 32) << 5)
    mov3 = 0xf2e00000 | (((image_size & 0xffff000000000000) >> 48) << 5)

    image[0x40:0x44] = mov0.to_bytes(4, 'little')
    image[0x44:0x48] = mov1.to_bytes(4, 'little')
    image[0x48:0x4c] = mov2.to_bytes(4, 'little')
    image[0x4c:0x50] = mov3.to_bytes(4, 'little')

    image[0x0c:0x14] = image_size.to_bytes(8, 'little')

    bzimage = f"{prefix_build}/vxgos.bzImage"
    if os.path.exists(bzimage):
        os.remove(bzimage)
    with open(bzimage, 'xb') as bzimage:
        bzimage.write(image)
