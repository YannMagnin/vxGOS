"""
core.font.meta - TOML meta information file abstraction
"""

from core.logger import log

__all__ = [
    'font_meta_fetch',
]

#---
# Public
#---

def font_meta_fetch(asset):
    """ Check and fetch font information

    @arg
    > asset (VxAsset) - asset information

    @return
    > dictionary with font information
    """
    # generate font default information
    font_info = {
        # user can customise
        'charset'         : 'normal',
        'grid_size_x'     : 0,
        'grid_size_y'     : 0,
        'grid_padding'    : 1,
        'grid_border'     : 0,
        'is_proportional' : False,
        'line_height'     : 0,
        'char_spacing'    : 1,

        # generated "on-the-fly" by the conversion step
        # @notes
        # This is mainly to provide cache for the Vhex operating system to
        # speed-up render calculation by avoiding recurent caculation.
        'glyph_size'      : 0,
        'glyph_height'    : 0,
        'font_size'       : 0,
        'data'            : []
    }

    # handle user meta-indication
    if 'charset' in asset.meta:
        if asset.meta['charset'] not in ['default', 'unicode']:
            log.error(f"Unknown charset '{asset.meta['charset']}', abord")
            return None
        font_info['charset'] = asset.meta['charset']
    if 'grid_size' not in asset.meta:
        log.error("Missing critical grid size information, abord")
        return None
    grid_size = asset.meta['grid_size'].split('x')
    font_info['grid_size_x'] = int(grid_size[0])
    font_info['grid_size_y'] = int(grid_size[1])
    if 'grid_padding' in asset.meta:
        font_info['grid_padding'] = int(asset.meta['grid_padding'])
    if 'grid_border' in asset.meta:
        font_info['grid_border'] = int(asset.meta['grid_border'])
    if 'proportional' in asset.meta:
        font_info['is_proportional'] = asset.meta['proportional']
    font_info['line_height'] = font_info['grid_size_y']
    if 'line_height' in asset.meta:
        font_info['line_height'] = asset.meta['line_height']
    if 'char_spacing' in asset.meta:
        font_info['char_spacing'] = asset.meta['char_spacing']
    font_info['glyph_height'] = font_info['grid_size_y']

    # return font information
    return font_info
