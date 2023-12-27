"""
sdk - vxGOS SDK entry
"""
import sys

from cli import cli_parse

#---
# Internals
#---

def _main(argv):
    """ real entry """
    if argv[0].find('conv') == 0:
        return cli_parse(argv)
    print(f"not supported {argv[0]}", file=sys.stderr)
    return -1

#---
# Public
#---

sys.exit(_main(sys.argv[1:]))
