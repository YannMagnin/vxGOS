"""
cli.conv.assets - Vhex asset converter user interface
"""
import os
import dataclasses

from core.logger import log
from core import assets_generate

__all__ = [
    'assets_conv_cli'
]

#---
# Internals
#---

__HELP__ = """vxsdk-converter-asset
Convert all assets file in the project directory.

USAGE:
    vxsdk vxgos conv-asset <assets prefix> <output prefix>

DESCRIPTION:
    Convert all assets file in the asset directory. This part of the converter
    module will scan the provided folder (or the current working directory) and
    will try to find all `vxconv.txt` file, which describe all assets that
    should be converted.

    If no argument is provided, then the current working directory is used as
    asset prefix and a storag for all generated source file. You can modify
    this behaviour using OPTIONS.

    For more information about the `vxconv.txt` in the wiki.

OPTIONS:
    -f, --force         Force assets generation even if they exist
        --bootloader    Generate bootloader font encoding
        --kernel        Generate kernel font encoding
        --os            Generate OS font encoding (default)
        --little-endian Encode assets information in little endian format
        --big-endian    Encode assets information in big endian format
    -h, --help          Display this help
"""

@dataclasses.dataclass
class _ConvertAssetInfo():
    """ Assets converter information """
    force         = False
    endianness    = 'little'
    generator     = 'os'
    prefix_output = ''
    prefix_assets = ''
    assets_filter = []

#---
# Public
#---

def assets_conv_cli(argv):
    """Process CLI arguments"""
    # check obvious flags
    if '-h' in argv or '--help' in argv:
        log.user(__HELP__)
        return 0

    # fetch user indication
    info = _ConvertAssetInfo()
    for arg in argv:
        if arg in ['-f', '--force']:
            info.force = True
            continue
        if arg == '--bootloader':
            info.generator = 'bootloader'
            continue
        if arg == '--little-endian':
            info.endianness = 'little'
            continue
        if arg == '--big-endian':
            info.endianness = 'big'
            continue
        if not info.prefix_assets:
            info.prefix_assets = arg
            continue
        if arg.find('--filter=') == 0:
            info.assets_filter = arg[9:].split(',')
            continue
        if info.prefix_output:
            log.warn(f"previous output path ({info.prefix_output}) dropped")
        info.prefix_output = arg

    # check indication
    if not info.prefix_assets:
        log.emergency('missing assets prefix')
    if not info.prefix_assets:
        log.emergency('missing output prefix')
    info.prefix_assets = os.path.abspath(info.prefix_assets)
    info.prefix_output = os.path.abspath(info.prefix_output)

    # generate assets information
    return assets_generate(info)
