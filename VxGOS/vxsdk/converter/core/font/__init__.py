"""
core.conv.type.font - Vhex font converter
"""
import os

from core.logger import log
from core.font.meta import font_meta_fetch
from core.font.convert import font_convert
from core.font.generator import font_generate_source_file

__all__ = [
    'font_generate'
]

#---
# Public
#---

def font_generate(asset, prefix_output, generator, endian, force_generate):
    """ Convert an image asset to a C source file

    @args
    > asset         (_VxAsset) - minimal asset information
    > prefix_output      (str) - prefix for source file generation
    > generator          (str) - selected generator for C file content
    > endian             (str) - selected endianness encoding
    > force_generate    (bool) - force generate the source file

    @return
    > pathname of the generated file
    """
    # check if the asset already exists
    asset_src = f'{prefix_output}/{asset.name}_vxfont.c'
    if not force_generate and os.path.exists(asset_src):
        log.debug(f"{asset.name}: not generated, cached")
        return asset_src

    # generate font information
    if not (font_info := font_meta_fetch(asset)):
        return ''
    if font_convert(asset, font_info) != 0:
        return ''

    # create the source file
    content = font_generate_source_file(asset, font_info, generator, endian)
    with open(asset_src, "w", encoding='utf8') as file:
        file.write(content)
    log.debug(f"source file generated at {asset_src}")
    return asset_src
