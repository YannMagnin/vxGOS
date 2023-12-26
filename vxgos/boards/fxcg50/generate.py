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
        for i in range(0, int(sec[4], base=16), 4):
            print(f"  > reloc GOT entry {got_base_addr+i:08x}...")
            symfile.write((got_base_addr + i).to_bytes(4, 'big'))
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
    cmd = f"sh-elf-vhex-objcopy -O binary -R .bss {binpath} {binpath}.raw"
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
            # direct 32bits address
            if sym[2] == 'R_SH_DIR32':
                print(f"  > reloc 'R_SH_DIR32' sym at {sym[0]}")
                symfile.write(int(sym[0], base=16).to_bytes(4, 'big'))
                continue
            # GOT related jump
            if sym[2] == 'R_SH_GOT32':
                need_patch_got = True
                continue
            # Other
            if sym[2] not in [
                'R_SH_PLT32',
                'R_SH_GOT32',
                'R_SH_GOTPC',
                'R_SH_GOTOFF'
            ]:
                print(f"  > [{i}] reloc {sym[2]} not supported")
                sys.exit(84)
        if need_patch_got:
            _patch_got_section(symfile, sectab)
        symfile.write(int('00000000', base=16).to_bytes(4, 'big'))

    print('- generate the full bootloader...')
    if os.path.exists(f"{binpath}.bzImage"):
        os.remove(f"{binpath}.bzImage")
    with open(f"{binpath}.bzImage", 'xb') as bzimgfile:
        with open(f"{binpath}.raw", 'rb') as rawbinfile:
            bzimgfile.write(rawbinfile.read())
        with open(f"{binpath}.symtab", 'rb') as symtabfile:
            bzimgfile.write(symtabfile.read())
    return f"{binpath}.bzImage"

def generate_image(prefix_build, bootloader_path, _):
    """ generate complet image (g3a) file

    TODO
    """

    image = bytearray(0)
    with open(bootloader_path, 'rb') as bootloaderfile:
        image += bootloaderfile.read()
    # (todo) os/kernel
    image_size = len(image)

    # patch first two instruction of the image (see <vxgos/bootloader>)
    image[0]  = 0b00000000   # (MSB) nop
    image[1]  = 0b00001001   # (LSB) nop
    image[2]  = 0b11010000   # (MSB) mov.l @(1*, PC), r0
    image[3]  = 0b00000001   # (LSB) mov.l @(1*, PC), r0
    image[8]  = (image_size & 0xff000000) >> 24
    image[9]  = (image_size & 0x00ff0000) >> 16
    image[10] = (image_size & 0x0000ff00) >> 8
    image[11] = (image_size & 0x000000ff) >> 0

    bzimage = f"{prefix_build}/vxgos.bzImage"
    if os.path.exists(bzimage):
        os.remove(bzimage)
    with open(bzimage, 'xb') as bzimage:
        bzimage.write(image)
    prefix = os.path.dirname(__file__)
    subprocess.run(
        [
            '/usr/bin/env',
            'python3',
            f"{prefix}/g3a_generator.py",
            f"{prefix_build}/vxgos.g3a",
            f"{prefix_build}/vxgos.bzImage",
            f"{prefix}/icon-sel.png",
            f"{prefix}/icon-uns.png",
        ],
        capture_output=False,
        check=False
    )
