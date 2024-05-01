"""
g1a_generator - Casio G1A file generator
"""
import os
import sys

from datetime import datetime
from PIL import Image

#---
# Internals
#---

## encoding helpers

def _u32(ptr: bytearray, idx: int, data: int) -> None:
    """ unsigned 32bits wrapper
    """
    ptr[idx + 0] = (data & 0xff000000) >> 24
    ptr[idx + 1] = (data & 0x00ff0000) >> 16
    ptr[idx + 2] = (data & 0x0000ff00) >> 8
    ptr[idx + 3] = (data & 0x000000ff) >> 0

def _u16(ptr: bytearray, idx: int, data: int) -> None:
    """ unsigned 16bits injection
    """
    ptr[idx + 0] = (data & 0xff00) >> 8
    ptr[idx + 1] = (data & 0x00ff) >> 0

def _u08(ptr: bytearray, idx: int, data: int) -> None:
    """ unsigned 8bits injection
    """
    ptr[idx + 0] = (data & 0xff) >> 0

def _str(ptr: bytearray, idx: int, data: str) -> None:
    """ string copy
    """
    for i, j in enumerate(data):
        _u08(ptr, idx + i, ord(j))

## g1a functions

def _generate_standar_header(addin: bytearray) -> None:
    """ generate Casio file standard header
    """
    _str(addin, 0x000, "USBPower")      # watermark
    _u08(addin, 0x008, 0xf3)            # addin file type
    _u08(addin, 0x009, 0x00)            # watermark
    _u08(addin, 0x00a, 0x01)            # watermark
    _u08(addin, 0x00b, 0x00)            # watermark
    _u08(addin, 0x00c, 0x01)            # watermark
    _u08(addin, 0x00d, 0x00)            # watermark
    _u08(addin, 0x00e, 0x00)            # LSB of file size + 0x41 (post)
    _u08(addin, 0x00f, 0x01)            # watermark
    _u32(addin, 0x010, 0x00000000)      # file size (MSB) (post)
    _u08(addin, 0x014, 0x00)            # LSB of file size + 0xb8 (post)
    _u16(addin, 0x016, 0x0000)          # sum of 8 words starting at 0x7100

def _generate_addin_subheader(addin: bytearray) -> None:
    """ generate the g1a addin subheader
    """
    _str(addin, 0x020, "@VXGOS  ")          # checksum (post)
    _u08(addin, 0x02b, 0x00)                # number of estrips
    _str(addin, 0x030, "01.00.0000")        # addin version
    _str(addin, 0x03c, datetime.now().strftime("%Y.%m%d.%M%S")) # date
    for i in range(0, 44):
        _u08(addin, 0x04c + i, 0x00)        # icon
    _str(addin, 0x1d4, "VXGOS   ")          # add name
    _u32(addin, 0x002e, 0x00000000)         # filesize (post)

def _generate_addin_icon(addin: bytearray, icon_path: str) -> None:
    """ encode mono icon in addin blob
    """
    with Image.open(icon_path) as icon:
        if icon.width != 30 and icon.height != 19:
            print(f"{icon_path}: image size should be 30x19 pixels, ignored")
            return
        pixels = icon.getdata()
    for _y in range(1, 18):
        for _x in range(0, 30):
            if pixels[(_y * 30) + _x] != (0, 0, 0, 255):
                continue
            _idx = 0x4c + ((_y * 4) + int((_x / 8)))
            _u08(addin, _idx, addin[_idx] | (0x80 >> (_x & 7)))

def _generate_checksums(addin: bytearray) -> None:
    """ generate all checksums requested for the g1a format
    """
    filesize = len(addin)
    _u08(addin, 0x010, (filesize & 0xff000000) >> 24)
    _u08(addin, 0x011, (filesize & 0x00ff0000) >> 16)
    _u08(addin, 0x012, (filesize & 0x0000ff00) >> 8)
    _u08(addin, 0x013, (filesize & 0x000000ff) >> 0)
    _u08(addin, 0x00e, addin[0x013] + 0x41)
    _u08(addin, 0x014, addin[0x013] + 0xb8)
    checksum = 0x00000
    for i in range(0, 8):
        checksum += int(
            addin[0x300 + (i * 2) : 0x302 + (i * 2)].hex(),
            base=16
        )
    _u16(addin, 0x016, checksum)
    _u08(addin, 0x1f0, (filesize & 0xff000000) >> 24)
    _u08(addin, 0x1f1, (filesize & 0x00ff0000) >> 16)
    _u08(addin, 0x1f2, (filesize & 0x0000ff00) >> 8)
    _u08(addin, 0x1f3, (filesize & 0x000000ff) >> 0)

def _main(argv: list[str]) -> int:
    """ main entry of the project
    """
    if len(argv) != 3:
        print('missing argument (./g1a_generator <output> <rawbin> <icon>)')
        sys.exit(84)
    rawbin = b''
    with open(argv[1], 'rb') as rawfile:
        rawbin = rawfile.read()
    addin = bytearray(0x200)
    _generate_standar_header(addin)
    _generate_addin_subheader(addin)
    _generate_addin_icon(addin, argv[2])
    addin += rawbin
    _generate_checksums(addin)
    if os.path.exists(argv[0]):
        os.remove(argv[0])
    with open(argv[0], 'xb') as addinfile:
        addinfile.write(addin)
    return 0

#---
# Public
#---

if __name__ == '__main__':
    sys.exit(_main(sys.argv[1:]))
