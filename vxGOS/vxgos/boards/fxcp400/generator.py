"""
board.fxcp400.generator - various generator (ASLR and final image)
"""
__all__ = [
    'generate_aslr_blob',
    'generate_final_image',
]
from typing import Optional, Any
from pathlib import Path
import sys

from vxsdk.core.utils import utils_cmd_exec
from vxsdk.core.logger import log

#---
# Internals
#---

def _patch_got_section(symfile: Any, section: list[str]) -> None:
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

def generate_aslr_blob(
    project_name:   str,
    prefix_build:   Path,
    elf_file:       Path,
    symtab:         list[Any],
    sectab:         list[Any],
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
        'sh-elf-vhex-objcopy '
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

    print('- generate the bootloader blob...')
    bzimg_file.unlink(missing_ok=True)
    with open(bzimg_file, 'xb') as bzimgfile:
        with open(raw_file, 'rb') as rawbinfile:
            bzimgfile.write(rawbinfile.read())
        with open(symtab_file, 'rb') as symtabfile:
            bzimgfile.write(symtabfile.read())
    return bzimg_file

def generate_final_image(
    prefix_build:       Path,
    bootloader_path:    Path,
    kernel_path:        Optional[Path] = None,
    os_path:            Optional[Path] = None,
) -> Path:
    """ generate complet raw kernel image
    """
    log.user('- construct the raw final image...')
    image = bytearray(0)
    for project_path in (bootloader_path, kernel_path, os_path):
        if not project_path:
            continue
        with open(project_path, 'rb') as projectfile:
            image += projectfile.read()
    image_size = len(image)

    log.user('- patching first two instruction of the image...')
    image[0]  = 0b00000000   # (MSB) nop
    image[1]  = 0b00001001   # (LSB) nop
    image[2]  = 0b11010000   # (MSB) mov.l @(1, PC), r0
    image[3]  = 0b00000001   # (LSB) mov.l @(1, PC), r0
    image[8]  = (image_size & 0xff000000) >> 24
    image[9]  = (image_size & 0x00ff0000) >> 16
    image[10] = (image_size & 0x0000ff00) >> 8
    image[11] = (image_size & 0x000000ff) >> 0

    log.user('- generating the kernel image (final)...')
    bzimage_path = prefix_build/'vxgos.img'
    bzimage_path.unlink(missing_ok=True)
    with open(bzimage_path, 'xb') as bzimage:
        bzimage.write(image)
    return bzimage_path
