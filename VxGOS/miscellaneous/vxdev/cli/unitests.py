"""
cli.unitests - unit tests interface
"""
import sys

from core.logger import log
from core.unitests import VxOSUnitest

__all__ = [
    'unitests_entry'
]

#---
# Internal
#---

__HELP__ = """ usage: vxdev unitest [OPTIONS] ...

Build script for the Vhex kernel

Options:
    -h, --help      display this message
    -v, --verbose   display more information during the building process
"""

#---
# Public
#---

def unitests_entry(argv):
    """ Build entry """
    if '-h' in argv or '--help' in argv:
        log.user(__HELP__)
        sys.exit(0)

    enable_verbose = False
    for arg in argv:
        if arg in ['-v', '--verbose']:
            enable_verbose = True
            continue
        log.error(f"unrecognized argument '{arg}'")

    unitest = VxOSUnitest(enable_verbose)
    unitest.configure()
    unitest.execute()
    return 0
