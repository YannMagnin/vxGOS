"""
core.font.convert - common conversion handling
"""

from PIL import Image

from core.logger import log
from core.font.glyph import glyph_get_wgeometry, glyph_encode

__all__ = [
    'font_convert',
]

#---
# Internals
#---

def _font_convert_proportional(packed_info):
    """ Generate proportional font

    Proportional font means that each character have its own width size (but
    have a common height). We need to performs more complexe handling than the
    monospaced one.

    @args
    > asset            (VxAsset) - asset information
    > font_information    (dict) - font indication

    @return
    > 0 if success, negative value otherwise
    """
    # unpack information
    font_info     = packed_info[0]
    img_info      = packed_info[1]
    glyph_info    = packed_info[2]
    data_info     = packed_info[4]
    geometry_info = packed_info[5]

    # isolate needed information
    img     = img_info['obj']
    img_raw = img_info['raw']
    nb_col  = packed_info[3][0]
    nb_row  = packed_info[3][1]
    gwidth  = glyph_info[0]
    gheight = glyph_info[1]

    # main loop, walk glyph per glyph
    _py = (font_info['grid_border'] + font_info['grid_padding']) * img.size[0]
    for _ in range(0, nb_row):
        _px = font_info['grid_border'] + font_info['grid_padding']
        for _ in range(0, nb_col):
            # generate width geometry information
            glyph_get_wgeometry(
                geometry_info,
                img_raw,
                img.size,
                (_px, _py),
                (font_info['grid_size_x'], font_info['grid_size_y'])
            )

            # save critical glyph geometry information that will be encoded in
            # the final C source file
            font_info['glyph_props'].append((
                geometry_info['wend'] - geometry_info['wstart'],
                data_info['idx'],
                data_info['shift']
            ))

            # encode glyph information
            glyph_encode(data_info, img_info, geometry_info, _px, _py)

            # update loop information
            font_info['glyph_count'] += 1
            _px += gwidth
        _py += gheight * img.size[0]
    return 0

def _font_convert_monospaced(packed_info):
    """ Generate proportional font

    Proportional font means that each character have its own width size (but
    have a common height). We need to performs more complexe handling than the
    monospaced one.

    @args
    > asset            (VxAsset) - asset information
    > font_information    (dict) - font indication

    @return
    > 0 if success, negative value otherwise
    """
    # unpack information
    font_info     = packed_info[0]
    img_info      = packed_info[1]
    glyph_info    = packed_info[2]
    grid_info     = packed_info[3]
    data_info     = packed_info[4]
    geometry_info = packed_info[5]

    # isolate needed information
    img     = img_info['obj']
    nb_row  = grid_info[1]
    nb_col  = grid_info[0]
    gwidth  = glyph_info[0]
    gheight = glyph_info[1]

    # main loop, walk glyph per glyph
    _py = (font_info['grid_border'] + font_info['grid_padding']) * img.size[0]
    for _ in range(0, nb_row):
        _px = font_info['grid_border'] + font_info['grid_padding']
        for _ in range(0, nb_col):
            glyph_encode(data_info, img_info, geometry_info, _px, _py)
            font_info['glyph_count'] += 1
            _px += gwidth
        _py += gheight * img.size[0]
    return 0

#---
# Public
#---

def font_convert(asset, font_info):
    """ Generate font information

    @args
    > asset     (VxAsset) - asset information
    > font_info    (dict) - font information

    @return
    > 0 if success, negative value otherwise
    """
    # generate image information
    img = Image.open(asset.path)
    img_raw = img.getdata()
    img_info = {
        'obj' : img,
        'raw' : img_raw
    }

    # pre-calculate the "real" glyph width and height using padding information
    glyph_info = [0, 0]
    glyph_info[0] = font_info['grid_size_x'] + font_info['grid_padding']
    glyph_info[1] = font_info['grid_size_y'] + font_info['grid_padding']
    gheight = glyph_info[1]
    gwidth = glyph_info[0]
    log.debug(f"gwidth = {gwidth} && gheight = {gheight}")

    # pre-calculate the number of row and column of the font
    grid_info = [0, 0]
    grid_info[0] = int((img.size[0] - (font_info['grid_border'] * 2)) / gwidth)
    grid_info[1] = int((img.size[1] - (font_info['grid_border'] * 2)) /gheight)
    nb_col = grid_info[0]
    nb_row = grid_info[1]
    log.debug(f"nb_row = {nb_row} && nb_col = {nb_col}")

    # pre-calculate and prepare per-glyph information
    # @note
    # The generated data is designed for 4-alignement padding. This to have
    # speed-up on drawing function.
    font_info['glyph_size']  = font_info['grid_size_x']
    font_info['glyph_size'] *= font_info['grid_size_y']
    font_info['font_size']   = font_info['glyph_size'] * nb_row * nb_col
    font_info['glyph_count'] = 0
    font_info['glyph_props'] = []
    font_info['data']        = [0] * int((font_info['font_size'] + 31) / 32)
    log.debug(f"data original = {id(font_info['data'])}")

    # generate data information
    data_info = {
        'table' : font_info['data'],
        'idx'   : 0,
        'shift' : 0
    }
    log.debug(f"data packed = {id(data_info['table'])}")

    # generate geometry information
    geometry_info = {
        'hstart' : 0,
        'hend'   : font_info['grid_size_y'],
        'wstart' : 0,
        'wend'   : font_info['grid_size_x'],
    }

    # select the converter
    converter = _font_convert_monospaced
    if font_info['is_proportional']:
        converter = _font_convert_proportional

    # convert font
    converter((
        font_info,
        img_info,
        glyph_info,
        grid_info,
        data_info,
        geometry_info
    ))

    log.debug(f"data packed end = {id(data_info['table'])}")
    return 0
