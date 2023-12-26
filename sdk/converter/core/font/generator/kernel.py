"""
core.font.generator.kernel - kernel font generator
"""

from core.logger import log

#---
# Public
#---

def _font_generate_unicode_source(_):
    """ Unicode special chaset directory """
    log.error("unicode conversion not implemented yet o(x_x)o")
    return ''

def _font_generate_normal_source(font_info):
    """ Print chaset is a image file
    """
    content  =  "\t.glyph = {\n"
    content += f"\t\t.height      = {font_info['glyph_height']},\n"
    content += f"\t\t.line_height = {font_info['line_height']},\n"

    # encode font bitmap
    line = 0
    log.debug(f"data = {font_info['data']}")
    content +=  "\t\t.data        = (uint32_t[]){\n"
    for pixel in font_info['data']:
        if line == 0:
            content += '\t\t\t'
        if line >= 1:
            content += ' '
        content += f"{pixel:#010x},"
        if (line := line + 1) == 4:
            content += '\n'
            line = 0
    if line != 0:
        content += '\n'
    content +=  '\t\t},\n'

    # indicate the number of glyph in the bitmap
    content += f"\t\t.count  = {font_info['glyph_count']},\n"

    # encode proportional information if needed
    if font_info['is_proportional']:
        content +=  '\t\t.prop = (struct __workaround[]){\n'
        for prop in font_info['glyph_props']:
            content +=  "\t\t\t{\n"
            content += f"\t\t\t\t.width = {prop[0]},\n"
            content += f"\t\t\t\t.index = {prop[1]},\n"
            content += f"\t\t\t\t.shift = {prop[2]},\n"
            content +=  "\t\t\t},\n"
    else:
        content +=  "\t\t.mono = {,\n"
        content += f"\t\t\t.width  = {font_info['glyph_width']},\n"
        content += f"\t\t\t.size   = {font_info['glyph_size']},\n"
    content += "\t\t},\n"
    content += "\t},\n"

    # skip unicode struct
    content +=  "\t.unicode = {\n"
    content +=  "\t\t.blocks      = NULL,\n"
    content +=  "\t\t.block_count = 0,\n"
    content +=  "\t}\n"
    return content

def _font_generate_source_file(asset, font_info):
    """ Generate font source file content

    @args
    > asset (VxAsset) - asset information
    > info     (dict) - hold font information

    @return
    > file C content string
    """
    # generate basic header
    content  =  "#include <vhex/display/font.h>\n"
    content +=  "\n"
    content += f"/* {asset.name} - Vhex asset\n"
    content +=  "   This object has been converted by using the vxSDK "
    content +=  "converter */\n"
    content += f"struct font const {asset.name} = " + "{\n"
    content += f"\t.name   = \"{asset.name}\",\n"

    # shape information
    content +=  "\t.shape  = {\n"
    content +=  "\t\t.bold   = 0,\n"
    content +=  "\t\t.italic = 0,\n"
    content +=  "\t\t.serif  = 0,\n"
    content +=  "\t\t.mono   = 0,\n"
    content += f"\t\t.prop   = {int(font_info['is_proportional'])},\n"
    content +=  "\t},\n"

    # manage display indication
    content += f"\t.char_spacing  = {font_info['char_spacing']},\n"

    # handle special charset behaviour
    if font_info['charset'] == 'unicode':
        content += _font_generate_unicode_source(font_info)
    else:
        content += _font_generate_normal_source(font_info)

    # closure and return
    content += '};\n'
    return content
