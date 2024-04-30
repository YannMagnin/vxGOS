"""
board.fx9860.generator  - various generator (ASLR and final image)
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
        for i in range(0, int(sec[4], base=16), 4):
            print(f"  > reloc GOT entry {got_base_addr+i:08x}...")
            symfile.write((got_base_addr + i).to_bytes(4, 'big'))
        break

def _patch_uint32(image: bytearray, offset: int, data: int) -> None:
    """ patch uint32_t at offset
    """
    image[offset + 0] = (data & 0xff000000) >> 24
    image[offset + 1] = (data & 0x00ff0000) >> 16
    image[offset + 2] = (data & 0x0000ff00) >> 8
    image[offset + 3] = (data & 0x000000ff) >> 0

#---
# Public
#---

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
    bzimg_file  = prefix_build/f"{project_name}.img"

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
    bootloader_blob = bytearray(0)
    with open(raw_file, 'rb') as rawbinfile:
        bootloader_blob += rawbinfile.read()
        _patch_uint32(bootloader_blob, 12, len(bootloader_blob))
        bootloader_blob[0]  = 0b11010000   # (MSB) mov.l @(2, PC), r0
        bootloader_blob[1]  = 0b00000010   # (LSB) mov.l @(2, PC), r0
    with open(symtab_file, 'rb') as symtabfile:
        bootloader_blob += symtabfile.read()
    bzimg_file.unlink(missing_ok=True)
    with open(bzimg_file, 'xb') as bzimgfile:
        bzimgfile.write(bootloader_blob)
    return bzimg_file

def generate_final_image(
    prefix_build:       Path,
    bootloader_path:    Path,
    kernel_path:        Optional[Path] = None,
    os_path:            Optional[Path] = None,
) -> Path:
    """ generate complet image (g3a) file
    """
    log.user('- construct the raw final image...')
    image = bytearray(0)
    kernel_info = [0, 0]
    for project_path in (bootloader_path, kernel_path, os_path):
        if not project_path:
            continue
        with open(project_path, 'rb') as projectfile:
            blob = projectfile.read()
        if project_path == kernel_path:
            kernel_info[0] = len(blob)
            kernel_info[1] = len(image)
        image += blob
    image_size = len(image)

    log.user('- patching first two instruction of the image...')
    image[2]  = 0b11010001   # (MSB) mov.l @(2, PC), r1
    image[3]  = 0b00000011   # (LSB) mov.l @(3, PC), r1
    image[4]  = 0b11010010   # (MSB) mov.l @(4, PC), r2
    image[5]  = 0b00000100   # (LSB) mov.l @(4, PC), r2
    image[6]  = 0b11010011   # (MSB) mov.l @(3, PC), r3
    image[7]  = 0b00000011   # (LSB) mov.l @(3, PC), r3
    image[16] = (image_size & 0xff000000) >> 24
    image[17] = (image_size & 0x00ff0000) >> 16
    image[18] = (image_size & 0x0000ff00) >> 8
    image[19] = (image_size & 0x000000ff) >> 0
    image[20] = (kernel_info[0] & 0xff000000) >> 24
    image[21] = (kernel_info[0] & 0x00ff0000) >> 16
    image[22] = (kernel_info[0] & 0x0000ff00) >> 8
    image[23] = (kernel_info[0] & 0x000000ff) >> 0
    image[24] = (kernel_info[1] & 0xff000000) >> 24
    image[25] = (kernel_info[1] & 0x00ff0000) >> 16
    image[26] = (kernel_info[1] & 0x0000ff00) >> 8
    image[27] = (kernel_info[1] & 0x000000ff) >> 0

    log.user('- generating the raw image...')
    img_path = prefix_build/'vxgos.img'
    img_path.unlink(missing_ok=True)
    with open(img_path, 'xb') as img:
        img.write(image)

    log.user('- generating the G1A final file...')
    prefix_board = str(Path(f"{__file__}/..").resolve())
    utils_cmd_exec(
        'python3 '
        f"{str(prefix_board)}/g1a_generator.py "
        f"{str(prefix_build)}/vxgos.g1a "
        f"{str(prefix_build)}/vxgos.img "
        f"{str(prefix_board)}/icon.bmp"
    )
    return prefix_build/'vxgos.g1a'
