"""
cli.conv.addin - vxSDK addin conversion
"""
import sys

#from core.conv.addin import generate_addin
from core.logger import log

__all__ = [
    'addin_conv_cli'
]

#---
# Internals
#---

__HELP__ = """vxsdk-converter-addin
Converte binary file into Vhex OS addin.

USAGE:
    vxsdk conv addin -b BINARY ...

DESCRIPTION:
    Convert a binary file into an application for the Vhex operating system.

OPTIONS:
    -b  <binary path>       ELF file (no check is performed in this file)
    -i <icon path>          92x62 pixel image path
    -o <output path>        output path for the generated addin
    -n <internal name>      internal addin name
    -v <internal version>   internal addin version
"""

#---
# Public
#---

def addin_conv_cli(argv):
    """Process CLI arguments"""
    if '-h' in argv or '--help' in argv:
        log.user(__HELP__)
        sys.exit(0)

    action = None
    info = [None, None, None, None, None]
    for arg in argv:
        if action == '-b':
            info[0] = arg
        if action == '-i':
            info[1] = arg
        if action == '-n':
            info[2] = arg
        if action == '-o':
            info[3] = arg
        if action == '-v':
            info[4] = arg
        if action:
            action = None
            continue
        if arg in ['-b', '-i', '-n', '-o', '-v']:
            action = arg
            continue

    if not info[0]:
        log.error('converter: need binary path !')
        sys.exit(84)

    log.error('not supported addin convertion')
    #return generate_addin(info[0], info[1], info[2], info[3], info[4])
