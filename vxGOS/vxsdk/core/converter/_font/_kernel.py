"""
vxsdk.core.converter._font._kernel  - kernel conv file generator
"""
from __future__ import annotations

__all__ = [
    'font_kernel_generate_source_file',
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

def _font_kernel_generate_mono(asset: ConvAssetFont) -> str:
    """ generate source file content for non-proportional font
    """
    content  =  '#include "vhex/display/font/types.h"\n'
    content +=  '\n'
    content += f"/* {asset.name} - Vhex asset\n"
    content +=  '   This object has been converted by using the vxSDK '
    content +=  'converter */\n'
    content += f"struct font const {asset.name} = " + "{\n"
    content += f"    .name = \"{asset.name}\",\n"
    content +=  '    .shape = {\n'
    content +=  '        .prop = 0,\n'
    content +=  '    },\n'
    content +=  '    .char_spacing = 1,\n'
    content +=  '    .glyph = {\n'
    content += f"        .count       = {asset.glyph_count},\n"
    content += f"        .height      = {asset.glyph_height},\n"
    content += f"        .line_height = {asset.line_height},\n"
    content +=  '        .mono = {\n'
    content += f"            .width           = {asset.grid_size_x},\n"
    content += f"            .char_block_size = {asset.glyph_size},\n"
    content +=  '        },\n'
    line = 0
    content +=  "        .data = (uint32_t[]){\n"
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
    content +=  '        },\n'
    content +=  '    },\n'
    content += '};\n'
    return content

#---
# Public
#---

def font_kernel_generate_source_file(
    asset: ConvAssetFont,
    _endianness: str,
) -> str:
    """ Generate font source file content
    """
    if asset.charset == 'unicode':
        log.emergency(f"{asset.name}: unsupported unicode font")
    if asset.is_proportional:
        log.emergency(f"{asset.name}: unsupported proportional font")
    return _font_kernel_generate_mono(asset)
