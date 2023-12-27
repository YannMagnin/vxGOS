"""
Expose configure script part
"""
from cli.build import build_entry
from cli.unitests import unitests_entry

__all__ = [
    'build',
    'unitests',
]

#---
# Public
#---

def build(argv):
    """ invoke build script """
    return build_entry(argv)

def unitests(argv):
    """ invoke unitests script """
    return unitests_entry(argv)
