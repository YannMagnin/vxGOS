"""
g3a_generator - Casio G3A file generator
"""
import os
import sys

from datetime import datetime
from PIL import Image

#---
# Internals
#---

def _u32(ptr, idx, data):
    """ unsigned 32bits wrapper """
    ptr[idx + 0] = (data & 0xff000000) >> 24
    ptr[idx + 1] = (data & 0x00ff0000) >> 16
    ptr[idx + 2] = (data & 0x0000ff00) >> 8
    ptr[idx + 3] = (data & 0x000000ff) >> 0

def _u16(ptr, idx, data):
    """ unsigned 16bits injection """
    ptr[idx + 0] = (data & 0xff00) >> 8
    ptr[idx + 1] = (data & 0x00ff) >> 0

def _u08(ptr, idx, data):
    """ unsigned 8bits injection """
    ptr[idx + 0] = (data & 0xff) >> 0

def _str(ptr, idx, data):
    """ string copy """
    for i, j in enumerate(data):
        _u08(ptr, idx + i, ord(j))

def _pxl(ptr, idx, pixel):
    """ convert Image pixel tuple to RGB565 data """
    _r = (pixel[0] & 0xff) >> 3
    _g = (pixel[1] & 0xff) >> 2
    _b = (pixel[2] & 0xff) >> 3
    _c = (_r << 11) | (_g << 5) | _b
    ptr[idx + 0] = (_c & 0xff00) >> 8
    ptr[idx + 1] = (_c & 0x00ff) >> 0

def _generate_standar_header(addin):
    """ generate Casio file standard header """
    _str(addin, 0x0000, "USBPower")         # watermark
    _u08(addin, 0x0008, 0x2c)               # addin file type
    _u08(addin, 0x0009, 0x00)               # watermark
    _u08(addin, 0x000a, 0x01)               # watermark
    _u08(addin, 0x000b, 0x00)               # watermark
    _u08(addin, 0x000c, 0x01)               # watermark
    _u08(addin, 0x000d, 0x00)               # watermark
    _u08(addin, 0x000e, 0x00)               # LSB of file size + 0x41 (post)
    _u08(addin, 0x000f, 0x01)               # watermark
    _u32(addin, 0x0010, 0x00000000)         # file size (MSB) (post)
    _u08(addin, 0x0014, 0x00)               # LSB of file size + 0xb8 (post)
    _u16(addin, 0x0016, 0x0000)             # 8 words sum starting at 0x7100

def _generate_addin_subheader(addin):
    """ generate the g3a addin subheader """
    _u32(addin, 0x0020, 0x00000000)         # checksum (post)
    _u16(addin, 0x0024, 0x0101)             # watermark
    _u32(addin, 0x002e, 0x00000000)         # filesize - 0x7000 - 4 (post)
    _str(addin, 0x0040, "VXOS")             # title padded with zeros
    _u32(addin, 0x005c, 0x00000000)         # file size (post)
    _str(addin, 0x0060, "@VXOS")            # internal name zero padded
    for i in range(0, 8):
        _str(addin, 0x006b + (i * 24), "vxOS")
    _u08(addin, 0x012b, 0x00)               # not usable by EAct
    _u32(addin, 0x012c, 0x00000000)         # watermark
    _str(addin, 0x0130, "01.00.0000")       # addin version
    _str(addin, 0x013c, datetime.now().strftime("%Y.%m%d.%M%S"))   # addin date
    _str(addin, 0x0ebc, "VxOS.g3a")         # filename

def _generate_checksums(addin):
    """ generate all checksums requested for the g3a format """
    filesize = len(addin)

    # standard header checksum
    _u08(addin, 0x010, (filesize & 0xff000000) >> 24)
    _u08(addin, 0x011, (filesize & 0x00ff0000) >> 16)
    _u08(addin, 0x012, (filesize & 0x0000ff00) >> 8)
    _u08(addin, 0x013, (filesize & 0x000000ff) >> 0)
    _u08(addin, 0x00e, addin[0x013] + 0x41)
    _u08(addin, 0x014, addin[0x013] + 0xb8)
    checksum = 0x00000
    for i in range(0, 8):
        checksum += int(
            addin[0x7100 + (i * 2) : 0x7102 + (i * 2)].hex(),
            base=16
        )
    _u16(addin, 0x016, checksum)

    # addin-specific header checksum
    _u32(addin, 0x02e, filesize - 0x7000 - 4)
    _u32(addin, 0x05c, filesize)
    checksum = 0x00000000
    for i in range(0x0000, 0x0020):
        checksum += addin[i]
    for i in range(0x0024, filesize - 4):
        checksum += addin[i]
    _u32(addin, 0x020, checksum)
    _u32(addin,    -4, checksum)

def _generate_addin_icon(addin, offset, icon_path):
    """ generate the g3a header icon (selected or unselected) """
    with Image.open(icon_path) as icon:
        if icon.width != 92 and icon.height != 64:
            print(f"{icon_path}: image size should be 64x24 pixels, ignored")
            return
        pixels = icon.getdata()
    _idx = 0
    for _y in range(0, 92):
        for _x in range(0, 64):
            _pxl(addin, offset + (_idx * 2), pixels[_idx])
            _idx += 1

def _main(argv):
    """ main entry of the project """
    if len(argv) != 4:
        print(
            'missing argument '
            '(./g3a_generator <output> <rawbin> <icon-sel> <icon-unsel>)'
        )
        sys.exit(84)

    rawbin = b''
    with open(argv[1], 'rb') as rawfile:
        rawbin = rawfile.read()

    addin = bytearray(0x7000)
    _generate_standar_header(addin)
    _generate_addin_subheader(addin)
    _generate_addin_icon(addin, 0x1000, argv[3])
    _generate_addin_icon(addin, 0x4000, argv[2])
    addin += rawbin
    addin += bytearray(4)
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
