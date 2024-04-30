"""
vxsdk.core.converter._font._bootloader   - bootloader generator
"""
from __future__ import annotations

__all__ = [
    'font_bootloader_generate_source_file',
]
from typing import TYPE_CHECKING

from vxsdk.core.logger import log

# bad design here
# @note
# The `ConvAssetFont` invoke this module with itself as a first argument,
# so, if we want to use proper typing information (and not just `Any`), we
# must use this dirty workaround to import the class only if a type
# checking is performed.
# However, we must import the special `__future__.annotations` which allow
# partial defined classes to be used as typing information.
if TYPE_CHECKING:
    from vxsdk.core.converter._font.font import ConvAssetFont

#---
# Internals
#---

## generation functions

def _font_bootloader_generate_normal_source(asset: ConvAssetFont) -> str:
    """ Print chaset is a image file
    """
    content  =  '#include "bootloader/display.h"\n'
    content +=  '\n'
    content += f"/* {asset.name} - Vhex asset\n"
    content +=  '   This object has been converted by using the vxSDK '
    content +=  'converter */\n'
    content += f"struct font const {asset.name} = " + "{\n"
    line = 0
    log.debug(f"data = {asset.data}")
    content +=  "    .data = (uint32_t[]){\n"
    for pixel in asset.data:
        if line == 0:
            content += '        '
        if line >= 1:
            content += ' '
        content += f"{pixel:#010x},"
        if (line := line + 1) == 6:
            content += '\n'
            line = 0
    if line != 0:
        content += '\n'
    content +=  '    },\n'
    content += f"    .count           = {asset.glyph_count},\n"
    content += f"    .height          = {asset.glyph_height},\n"
    content += f"    .width           = {asset.grid_size_x},\n"
    content += f"    .line_height     = {asset.line_height},\n"
    content += f"    .char_block_size = {asset.glyph_size},\n"
    content += '};\n'
    return content

#---
# Public
#---

def font_bootloader_generate_source_file(
    asset: ConvAssetFont,
    _endianness: str,
) -> str:
    """ Generate font source file content
    """
    if asset.is_proportional:
        log.emergency(f"{asset.name}: unsupported proportional font")
    if asset.charset == 'unicode':
        log.emergency(f"{asset.name}: unsupported unicode font")
    return _font_bootloader_generate_normal_source(asset)
