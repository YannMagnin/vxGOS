"""
core.unitests.bootloader - bootloader unit tests abstraction
"""
import os
import glob

from core.logger import log
from core.unitests.requirements import requirements_unitests_parse
from core.unitests.cmake import cmake_unitest_configure

__all__ = [
    'bootloader_unitest_config'
]

#---
# Internals
#---

def _bootloader_unitest_new(unitest, prefix, test):
    """ add a new bootloader unitest
    """
    log.debug(f"- new unitest '{test}'...")

    req = requirements_unitests_parse(prefix, 'bootloader', test)
    log.debug(f"> deplist = {req}")

    srclist = f"{prefix}/unitests/{test}/**/*.[csS]"
    srclist = glob.glob(srclist, recursive=True)
    log.debug(f"> srclist = {srclist}")

    include_list = [
        f"{prefix}/include/",
        f"{prefix}/unitests/{test}/"
    ]

    return {
        'name' : test,
        'prefix' : cmake_unitest_configure(
            f"{unitest.prefix['build']}/bootloader/{test}",
            {
                'name'    : test,
                'src'     : req['covered'] + req['dependencies'] + srclist,
                'include' : req['includes'] + include_list
            }
        ),
        'requirements' : req
    }

#---
# Public
#---

def bootloader_unitest_config(unitest):
    """ bootloader unitests abstraction entry
    """
    unitpath = []
    prefix = f"{unitest.prefix['src']}/bootloader"
    for test in os.listdir(f"{prefix}/unitests/"):
        unitpath.append(_bootloader_unitest_new(unitest, prefix, test))
    return unitpath
