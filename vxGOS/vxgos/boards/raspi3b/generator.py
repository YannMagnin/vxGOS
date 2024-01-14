"""
generate - post-build script used to generate bootlaoder blob with ASLR
"""
__all__ = [
    'generate_aslr_blob',
    'generate_final_image',
]
from typing import Optional, List, Any
from pathlib import Path
import sys

from vxsdk.core.utils import utils_cmd_exec
from vxsdk.core.logger import log

#---
# Internals
#---

def _patch_got_section(symfile: Any, section: List[str]) -> None:
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

# allow too many local variables declaractions
# pylint: disable=locally-disabled,R0914
def generate_aslr_blob(
    project_name:   str,
    prefix_build:   Path,
    elf_file:       Path,
    symtab:         List[Any],
    sectab:         List[Any],
) -> Path:
    """ generate bootloader final blob with ASLR

    The objectif of this script is to generate the final bootloader blob
    with ASLR. To performs this, we will performs 3 steps:

        * generate the raw binary file of the bootloader (objcpy)
        * generate the raw ALSR symbols table
        * generate the complet blootloader image

    And return the final blob path
    """
    raw_file    = prefix_build/f"{project_name}.raw"
    symtab_file = prefix_build/f"{project_name}.symtab"
    bzimg_file  = prefix_build/f"{project_name}.bzImage"

    print('- generate raw binary...')
    utils_cmd_exec(
        'aarch64-linux-gnu-objcopy '
        '-O binary '
        '-R .bss '
        f"{str(elf_file)} "
        f"{str(raw_file)}"
    )

    print('- generate raw symtab...')
    need_patch_got = False
    symtab_file.unlink(missing_ok=True)
    with open(symtab_file, 'xb') as symfile:
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
        symfile.write(
            int('0000000000000000', base=16).to_bytes(8, 'little')
        )

    print('- generate the full bootloader...')
    image_size = 0
    bzimg_file.unlink(missing_ok=True)
    with open(bzimg_file, 'xb') as bzimgfile:
        with open(raw_file, 'rb') as rawbinfile:
            content = rawbinfile.read()
            bzimgfile.write(content)
            image_size += len(content)
        with open(symtab_file, 'rb') as symtabfile:
            content = symtabfile.read()
            bzimgfile.write(content)
            image_size += len(content)
        if image_size % 512:
            for _ in range(0, 512 - (image_size % 512)):
                bzimgfile.write(0x00.to_bytes(1))
    return bzimg_file

def generate_final_image(
    prefix_build:       Path,
    bootloader_path:    Path,
    kernel_path:        Optional[Path] = None,
    os_path:            Optional[Path] = None,
) -> Path:
    """ generate complet image file
    """
    log.user('- construct the raw final image...')
    image = bytearray(0)
    for project_path in (bootloader_path, kernel_path, os_path):
        if not project_path:
            continue
        with open(project_path, 'rb') as projectfile:
            image += projectfile.read()
    image_size = len(image)

    image = bytearray(0)
    with open(bootloader_path, 'rb') as bootloaderfile:
        image += bootloaderfile.read()
    # (todo) os/kernel
    image_size = len(image)

    # @note
    # mov x0, #0x0000         = 0xd2800000  -  0xd29fffe0
    # mov x0, #0x0000, lsl 16 = 0xf2a00000  -  0xf2bfffe0
    # mov x0, #0x0000, lsl 32 = 0xf2c00000  -  0xf2dfffe0
    # mov x0, #0x0000, lsl 48 = 0xf2e00000  -  0xf2ffffe0
    log.user('- patching first two instruction of the image...')
    mov0 = 0xd2800000 | (((image_size & 0x000000000000ffff) >>  0) << 5)
    mov1 = 0xf2a00000 | (((image_size & 0x00000000ffff0000) >> 16) << 5)
    mov2 = 0xf2c00000 | (((image_size & 0x0000ffff00000000) >> 32) << 5)
    mov3 = 0xf2e00000 | (((image_size & 0xffff000000000000) >> 48) << 5)
    image[0x40:0x44] = mov0.to_bytes(4, 'little')
    image[0x44:0x48] = mov1.to_bytes(4, 'little')
    image[0x48:0x4c] = mov2.to_bytes(4, 'little')
    image[0x4c:0x50] = mov3.to_bytes(4, 'little')
    image[0x0c:0x14] = image_size.to_bytes(8, 'little')

    log.user('- generating the BzImage...')
    bzimage_path = prefix_build/'vxgos.bzImage'
    bzimage_path.unlink(missing_ok=True)
    with open(bzimage_path, 'xb') as bzimage:
        bzimage.write(image)
    return bzimage_path
