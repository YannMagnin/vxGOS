"""
core.conv.pixel - Pixel converter utilities
"""

__all__ = [
    'pixel_rgb24to16'
]

#---
# Public
#---

def pixel_rgb24to16(rgb):
    """ convert RGB24 -> RGB16-565"""
    _r = (rgb[0] & 0xff) >> 3
    _g = (rgb[1] & 0xff) >> 2
    _b = (rgb[2] & 0xff) >> 3
    return (_r << 11) | (_g << 5) | _b

#def rgb1conv(pixel):
#    """ Convert RGB<BLACK> -> 1-bit color """
#    return pixel == (0, 0, 0)

#def rgb8conv(pixel):
#    """ Convert RGB8 -> """
#    return int((pixel[0] * 7) / 255) << 5   \
#        | int((pixel[1] * 4) / 255) << 3    \
#        | int((pixel[2] * 7) / 255) << 0

#def rgba8conv(pixel):
#    return int((pixel[0] * 4) / 256) << 6   \
#        | int((pixel[1] * 8) / 256) << 3    \
#        | int((pixel[2] * 4) / 256) << 1    \
#        | (len(pixel) >= 4 and pixel[3] == 0)

#def rgb16conv(pixel):
#    return int((pixel[0] * 31) / 255) << 11   \
#            | int((pixel[1] * 63) / 255) << 5 \
#            | int((pixel[2] * 31) / 255) << 0

#def rgba16conv(pixel):
#    return int((pixel[0] * 31) / 255) << 11   \
#            | int((pixel[1] * 63) / 255) << 6 \
#            | int((pixel[2] * 31) / 255) << 1 \
#            | (pixel[3] != 0)
