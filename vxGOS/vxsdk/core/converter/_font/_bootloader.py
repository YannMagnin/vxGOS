"""
vxsdk.core.converter._font._bootloader   - bootloader generator
"""
__all__ = [
    'font_bootloader_generate_source_file',
]
from typing import Any

from vxsdk.core.logger import log

#---
# Internals
#---

## endian abstraction

def _u32_conv_little_endian(x32: int) -> int:
#    return ((x32 & 0xff000000) >> 24)   \
#            | ((x32 & 0x00ff0000) >> 8) \
#            | ((x32 & 0x0000ff00) << 8) \
#            | ((x32 & 0x000000ff) << 24)
    return x32

def _u32_conv_big_endian(x32: int) -> int:
    return x32

## generation functions

def _font_bootloader_generate_normal_source(
    asset: Any,
    endianness: str,
) -> str:
    """ Print chaset is a image file

    Generate a C font file content based on bootloader font internal
    structure and header declaration.

    @args
    > asset  (VxAsset) - asset information
    > info      (dict) - hold font information
    > endianness (str) - selected endianness encoding

    @return
    > complet font file content
    """
    # generate basic header
    content  =  '#include "bootloader/display.h"\n'
    content +=  '\n'
    content += f"/* {asset.name} - Vhex asset\n"
    content +=  '   This object has been converted by using the vxSDK '
    content +=  'converter */\n'
    content += f"struct font const {asset.name} = " + "{\n"

    # handle endianness
    u32 = _u32_conv_little_endian
    if endianness != 'little':
        u32 = _u32_conv_big_endian

    # encode font bitmap
    line = 0
    log.debug(f"data = {asset.data}")
    content +=  "    .data = (uint32_t[]){\n"
    for pixel in asset.data:
        if line == 0:
            content += '        '
        if line >= 1:
            content += ' '
        content += f"{u32(pixel):#010x},"
        if (line := line + 1) == 6:
            content += '\n'
            line = 0
    if line != 0:
        content += '\n'
    content +=  '    },\n'

    # indicate other font information
    content += f"    .count           = {asset.glyph_count},\n"
    content += f"    .height          = {asset.glyph_height},\n"
    content += f"    .width           = {asset.grid_size_x},\n"
    content += f"    .line_height     = {asset.line_height},\n"
    content += f"    .char_block_size = {asset.glyph_size},\n"

    # closure and return
    content += '};\n'
    return content

#---
# Public
#---

def font_bootloader_generate_source_file(
    asset: Any,
    endianness: str,
) -> str:
    """ Generate font source file content
    """
    if asset.is_proportional:
        log.emergency(f"{asset.name}: unsupported proportional font")
    if asset.charset == 'unicode':
        log.emergency(f"{asset.name}: unsupported unicode font")
    return _font_bootloader_generate_normal_source(asset, endianness)
