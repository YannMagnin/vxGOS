"""
core.font.generator - generator abstraction
"""

from core.logger import log
from core.font.generator.bootloader import font_generate_bootloader_source_file

#---
# Public
#---

def font_generate_source_file(asset, font_info, generator, endianness):
    """ return the C source file based on the selected generator

    @args
    > asset         (_VxAsset) - minimal asset information
    > font_info         (dict) - font information
    > generator          (str) - selected generator for C file content
    > endianness         (str) - selected endianness encoding

    @return
    > file content (str)
    """
    if generator not in ['bootloader', 'kernel', 'os']:
        log.emergency(f"not supported generator '{generator}'")

    if generator == 'bootloader':
        return font_generate_bootloader_source_file(
            asset,
            font_info,
            endianness
        )

    log.emergency(f"generator '{generator}' not implemented yet o(x_x)o")
    return ''
