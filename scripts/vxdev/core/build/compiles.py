"""
core.build.compiles - handle "compiles.toml" files
"""
import dataclasses
import toml

from core.logger import log

__all__ = [
    'compiles_fetch',
]

#---
# Internals
#---

@dataclasses.dataclass
class CompilesInfo():
    """ board compilation information """
    toolchain_prefix = ''
    toolchain_processor = ''
    toolchain_endian = ''
    cflags  = []
    ldflags = []
    libs    = []
    assets  = []

#---
# Public
#---

def compiles_fetch(filename):
    """ try to load the compiles.toml file
    """
    board_info = toml.load(filename)
    info = CompilesInfo()

    check  = 'VXDEV_TOOLCHAIN_PREFIX' in board_info
    check += 'VXDEV_TOOLCHAIN_PROCESSOR' in board_info
    check += 'VXDEV_TOOLCHAIN_ENDIANNESS' in board_info
    if check != 3:
        log.emergency(f"{filename}: missing toolchain information, abord")

    info.toolchain_prefix    = board_info['VXDEV_TOOLCHAIN_PREFIX']
    info.toolchain_processor = board_info['VXDEV_TOOLCHAIN_PROCESSOR']
    info.toolchain_endian    = board_info['VXDEV_TOOLCHAIN_ENDIANNESS']
    if 'VXDEV_CFLAGS' in board_info:
        info.cflags = board_info['VXDEV_CFLAGS']
    if 'VXDEV_LDFLAGS' in board_info:
        info.ldflags = board_info['VXDEV_LDFLAGS']
    if 'VXDEV_LIBS' in board_info:
        info.libs = board_info['VXDEV_LIBS']
    if 'VXDEV_ASSETS' in board_info:
        info.assets = board_info['VXDEV_ASSETS']

    return info
