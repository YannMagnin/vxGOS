"""
Vhex kernel developement script
"""
import sys

import cli
from core.logger import log

#---
# Internals
#---

__HELP__ = """ usage: vxdev [ACTION] ...

Actions:
    doctor      display information about the vxOS and supported feature
    build       perfrom build step
    install     perform installation step
    uninstall   perform uninstallation step

Options:
    -h,--help   display this message

You can try `vxdev <action> --help` for more information about each action
"""


def _main(argv):
    """ main entry of the script """
    if '-h' in argv or '--help' in argv:
        log.user(__HELP__)
        sys.exit(0)
    if not argv:
        log.user(__HELP__)
        sys.exit(84)

    if argv[0] in ['-v', '-vv', '-vvv']:
        log.level += len(argv[0]) - 1
        argv = argv[1:]
        if not argv:
            log.error(__HELP__)
            sys.exit(84)

    if argv[0] == 'build':
        sys.exit(cli.build(argv[1:]))
    if argv[0] == 'unitests':
        sys.exit(cli.unitests(argv[1:]))

    print(f"unrecognized argument '{argv[0]}'", file=sys.stderr)
    sys.exit(84)

#---
# Public
#---

sys.exit(_main(sys.argv[1:]))
