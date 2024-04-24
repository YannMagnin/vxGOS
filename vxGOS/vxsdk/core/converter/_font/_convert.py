"""
vxsdk.core.converter._font._convert - convert image to raw data
"""
__all__ = [
    'font_convert_img_to_raw',
]
from typing import Any

from PIL import Image

from vxsdk.core.logger import log
from vxsdk.core.converter._font._glyph import (
    font_glyph_get_wgeometry,
    font_glyph_encode,
)

# theoricaly, we should import the class definition for typing indication
# but since the said class manually invoke this module, we use `Any` as
# type to avoid circular import design error
#from vxsdk.core.converter._font.font import ConvAssetFont

#---
# Internals
#---

# since we use many cached information to speed-up the asset generation
# we allow internal function to have more than 5 arguments
# pylint: disable=locally-disabled,R0913

def _font_convert_proportional(
    asset:          Any,
    img_info:       dict[str,Any],
    glyph_info:     list[int],
    grid_info:      list[int],
    data_info:      dict[str,Any],
    geometry_info:  dict[str,Any],
) -> None:
    """ Generate proportional font

    Proportional font means that each character have its own width size (but
    have a common height). We need to performs more complexe handling than
    the monospaced one.
    """
    # isolate needed information
    img     = img_info['obj']
    img_raw = img_info['raw']
    nb_col  = grid_info[0]
    nb_row  = grid_info[1]
    gwidth  = glyph_info[0]
    gheight = glyph_info[1]

    # main loop, walk glyph per glyph
    _py = (asset.grid_border + asset.grid_padding) * img.size[0]
    for _ in range(0, nb_row):
        _px = asset.grid_border + asset.grid_padding
        for _ in range(0, nb_col):
            # generate width geometry information
            font_glyph_get_wgeometry(
                geometry_info,
                img_raw,
                img.size,
                (_px, _py),
                (asset.grid_size_x, asset.grid_size_y),
            )

            # save critical glyph geometry information that will be encoded
            # in the final C source file
            asset.glyph_props.append((
                geometry_info['wend'] - geometry_info['wstart'],
                data_info['idx'],
                data_info['shift']
            ))

            # encode glyph information
            font_glyph_encode(data_info, img_info, geometry_info, _px, _py)

            # update loop information
            asset.glyph_count += 1
            _px += gwidth
        _py += gheight * img.size[0]

def _font_convert_monospaced(
    asset:          Any,
    img_info:       dict[str,Any],
    glyph_info:     list[int],
    grid_info:      list[int],
    data_info:      dict[str,Any],
    geometry_info:  dict[str,Any],
) -> None:
    """ Generate proportional font
    """
    # isolate needed information
    img     = img_info['obj']
    nb_row  = grid_info[1]
    nb_col  = grid_info[0]
    gwidth  = glyph_info[0]
    gheight = glyph_info[1]

    # main loop, walk glyph per glyph
    _py = (asset.grid_border + asset.grid_padding) * img.size[0]
    for _ in range(0, nb_row):
        _px = asset.grid_border + asset.grid_padding
        for _ in range(0, nb_col):
            font_glyph_encode(data_info, img_info, geometry_info, _px, _py)
            asset.glyph_count += 1
            _px += gwidth
        _py += gheight * img.size[0]

#---
# Public
#---

def font_convert_img_to_raw(asset: Any) -> None:
    """ Generate font information
    """
    # generate image information
    img = Image.open(asset.image_path)
    img_raw = img.getdata()
    img_info = {
        'obj' : img,
        'raw' : img_raw
    }

    # pre-calculate the "real" glyph width and height using padding
    # information
    glyph_info = [0, 0]
    glyph_info[0] = asset.grid_size_x + asset.grid_padding
    glyph_info[1] = asset.grid_size_y + asset.grid_padding
    gheight = glyph_info[1]
    gwidth = glyph_info[0]
    log.debug(f"gwidth = {gwidth} && gheight = {gheight}")

    # pre-calculate the number of row and column of the font
    grid_info = [0, 0]
    grid_info[0] = int((img.size[0] - (asset.grid_border * 2)) / gwidth)
    grid_info[1] = int((img.size[1] - (asset.grid_border * 2)) /gheight)
    nb_col = grid_info[0]
    nb_row = grid_info[1]
    log.debug(f"nb_row = {nb_row} && nb_col = {nb_col}")

    # pre-calculate and prepare per-glyph information
    # @note
    # The generated data is designed for 4-alignement padding. This to have
    # speed-up on drawing function.
    asset.glyph_size  = asset.grid_size_x * asset.grid_size_y
    asset.font_size   = asset.glyph_size * nb_row * nb_col
    asset.glyph_count = 0
    asset.glyph_props = []
    asset.data        = [0] * int((asset.font_size + 31) / 32)
    log.debug(f"data original = {id(asset.data)}")

    # generate data information
    data_info = {
        'table' : asset.data,
        'idx'   : 0,
        'shift' : 0
    }
    log.debug(f"data packed = {id(data_info['table'])}")

    # generate geometry information
    geometry_info = {
        'hstart' : 0,
        'hend'   : asset.grid_size_y,
        'wstart' : 0,
        'wend'   : asset.grid_size_x,
    }

    # select the converter and convert the font
    converter = _font_convert_monospaced
    if asset.is_proportional:
        converter = _font_convert_proportional
    converter(
        asset,
        img_info,
        glyph_info,
        grid_info,
        data_info,
        geometry_info
    )
    log.debug(f"data packed end = {id(data_info['table'])}")
