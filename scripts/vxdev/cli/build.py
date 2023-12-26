"""
Vhex kernel build script
"""
import os
import sys

from core.logger import log
from core.build import VxOSBuild

__all__ = [
    'build_entry'
]

#---
# Internal
#---

__HELP__ = """ usage: vxdev build [OPTIONS] ...

Build script for the Vhex kernel

Options:
    -h, --help      display this message
    -v, --verbose   display more information during the building process
"""

#---
# Public
#---

def build_entry(argv):
    """ Build entry """
    if '-h' in argv or '--help' in argv:
        log.user(__HELP__)
        sys.exit(0)

    if 'VXSDK_PKG_TARGET' not in os.environ \
            or 'VXSDK_PREFIX_BUILD' not in os.environ \
            or 'VXSDK_PREFIX_INSTALL' not in os.environ:
        log.error("unable to build the vxGOS without using the vxSDK")

    enable_verbose = False
    for arg in argv:
        if arg in ['-v', '--verbose']:
            enable_verbose = True
            continue
        log.error(f"unrecognized argument '{arg}'")

    build = VxOSBuild(enable_verbose)
    build.configure()
    build.build()
    return 0
