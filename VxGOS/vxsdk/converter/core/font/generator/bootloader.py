"""
core.font.generator.bootloader - bootloader font generator
"""

from core.logger import log

__all__ = [
    'font_generate_bootloader_source_file'
]

#---
# Internals
#---

def _u32_conv_little_endian(x32):
#    return ((x32 & 0xff000000) >> 24)   \
#            | ((x32 & 0x00ff0000) >> 8) \
#            | ((x32 & 0x0000ff00) << 8) \
#            | ((x32 & 0x000000ff) << 24)
    return x32

def _u32_conv_big_endian(x32):
    return x32

def _font_generate_normal_source(asset, font_info, endianness):
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
    log.debug(f"data = {font_info['data']}")
    content +=  "    .data = (uint32_t[]){\n"
    for pixel in font_info['data']:
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
    content += f"    .count           = {font_info['glyph_count']},\n"
    content += f"    .height          = {font_info['glyph_height']},\n"
    content += f"    .width           = {font_info['grid_size_x']},\n"
    content += f"    .line_height     = {font_info['line_height']},\n"
    content += f"    .char_block_size = {font_info['glyph_size']},\n"

    # closure and return
    content += '};\n'
    return content

#---
# Public
#---

def font_generate_bootloader_source_file(asset, font_info, endianness):
    """ Generate font source file content

    @args
    > asset  (VxAsset) - asset information
    > info      (dict) - hold font information
    > endianness (str) - selected endianness encoding

    @return
    > file C content string
    """
    # check font validity (common vxconv.toml parser)
    if font_info['is_proportional']:
        log.emergency(f"{asset.name}: unsupported proportional font")
    if font_info['charset'] == 'unicode':
        log.emergency(f"{asset.name}: unsupported unicode font")

    # generate file content
    return _font_generate_normal_source(asset, font_info, endianness)
