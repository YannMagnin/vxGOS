"""
cli.project - handles project creation and more
"""
import sys

from core.project import new_project
from core.logger import log

__all__ = [
    '__VXSDK_MODULE_META__',
    'cli_validate',
    'cli_parse'
]

#---
# Public
#---

__VXSDK_MODULE_META__ = (
    ['p', 'project'],
    "project abstraction",
    r"""vxsdk-project
Abstract project manipulation

USAGE:
    vxsdk project <COMMAND> [OPTIONS]

OPTIONS:
    -h, --help              Print helps information

Common used commands:
    n, new                  Create a new project

See `vxsdk project <action> --help` for more information on a specific command
"""
)

def cli_validate(name):
    """ validate the module name """
    return name in __VXSDK_MODULE_META__[0]

def cli_parse(argv):
    """ Project subcommand entry """
    if len(argv) > 1:
        if '-h' in argv or '--help' in argv:
            log.user(__VXSDK_MODULE_META__[2])
            sys.exit(0)
    if len(argv) > 3:
        if argv[0] in ['n', 'new']:
            for path in argv[1:]:
                new_project(path)
            sys.exit(0)
    log.error(__VXSDK_MODULE_META__[2])
    sys.exit(84)
