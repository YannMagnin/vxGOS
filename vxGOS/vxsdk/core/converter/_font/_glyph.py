"""
vxsdk.core.converter._font._glyph   - glyph calculation helper
"""
__all__ = [
    'font_glyph_get_wgeometry',
    'font_glyph_encode',
]
from typing import Any

from vxsdk.core.logger import log

#---
# Public
#---

def font_glyph_get_wgeometry(
    geometry_info:  dict[str,Any],
    img_raw:        Any,
    img_size:       tuple[int,int],
    pos:            tuple[int,int],
    grid_size:      tuple[int,int],
) -> None:
    """ Generate glyph width geometry information

    @args
    > geometry_info (dict) - geometry information
    > img_raw       (list) - list of all pixel of the image
    > img_size     (tuple) - image width and image height
    > pos          (tuple) - glyph position information (X and Y in pixel)
    > grid_size    (tuple) - glyph grid size information (width and height)

    @return
    > Nothing
    """
    geometry_info['wstart'] = -1
    geometry_info['wend'] = -1

    _px = pos[0]
    _py = pos[1]
    log.debug(f'[geometry] X:{pos[0]} Y:{int(pos[1]/img_size[0])}')
    log.debug(f' - grid_size = {grid_size}')
    for _ in range(0, grid_size[1]):
        for offx in range(0, grid_size[0]):
            if img_raw[_py + (_px + offx)][:3] == (255, 255, 255):
                continue
            if (
                   geometry_info['wstart'] < 0    \
                or offx < geometry_info['wstart'] \
            ):
                geometry_info['wstart'] = offx
            if geometry_info['wstart'] < 0 or offx > geometry_info['wend']:
                geometry_info['wend'] = offx
        _py += img_size[0]
    geometry_info['wend'] += 1
    log.debug(f' - geometry = {geometry_info}')

def font_glyph_encode(
    data_info:  dict[str,Any],
    img_info:   dict[str,Any],
    geometry:   dict[str,Any],
    posx:       int,
    posy:       int,
) -> None:
    """ Encode glyph bitmap

    @args
    > data_info (dict) - internal data information (list, index and shift)
    > img_info  (dict) - image-related information (object and raw content)
    > geometry  (dict) - geometry information
    > posx       (int) - X-axis position in pixel
    > posy       (int) - Y-axis position in pixel

    @return
    > Nothing
    """
    # fetch information
    img        = img_info['obj']
    img_raw    = img_info['raw']
    data       = data_info['table']
    data_idx   = data_info['idx']
    data_shift = data_info['shift']
    wstart     = geometry['wstart']
    wend       = geometry['wend']

    # encode the glyph
    yoff = 0
    log.debug(f'[encode] X:{posx} Y:{int(posy/img.size[0])}')
    for _h in range(geometry['hstart'], geometry['hend']):
        for _w in range(wstart, wend):
            if img_raw[(posy + yoff) + (posx + _w)][:3] == (0, 0, 0):
                log.debug('#', end='')
                data[data_idx] |= 0x80000000 >> data_shift
            else:
                log.debug('.', end='')
                data[data_idx] &= ~(0x80000000 >> data_shift)
            if (data_shift := data_shift + 1) >= 32:
                data_shift = 0
                data_idx += 1
        log.debug('')
        yoff += img.size[0]

    # commit modification
    data_info['idx'] = data_idx
    data_info['shift'] = data_shift
