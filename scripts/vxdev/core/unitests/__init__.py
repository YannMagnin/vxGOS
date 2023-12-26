"""
core.unitests - unit tests abstraction
"""
import os
import subprocess

from core.logger import log
from core.unitests.bootloader import bootloader_unitest_config
from core.unitests.cmake import cmake_unitest_compile
from core.unitests.criterion import criterion_unitests_run
from core.unitests.gcovr import gcovr_unitests_run

__all__ = [
    'VxOSUnitest'
]

#---
# Public
#---

class VxOSUnitest():
    """ Unit tests abstraction
    """
    def __init__(self, verbose):
        self._verbose = verbose
        prefix_base   = f"{os.path.dirname(__file__)}/../../../../"
        prefix_base   = os.path.normpath(prefix_base)
        self._prefix  = {
            'base'  : prefix_base,
            'build' : f"{prefix_base}/.vxsdk/unitests",
            'src'   : f"{prefix_base}/vxgos",
        }
        self._unitpath = {
            'bootloader' : [],
            'kernel'     : [],
            'os'         : [],
        }

    #---
    # Properties
    #---

    @property
    def prefix(self):
        """ return the prefix dictionary """
        return self._prefix

    #---
    # Methods
    #---

    def configure(self):
        """ generate all unitests env """
        self._unitpath['bootloader'] = bootloader_unitest_config(self)

    def execute(self):
        """ execute all unitests
        """
        bininfo_list = []
        log.user('- compile unitests...')
        for item in self._unitpath.items():
            for unitest in item[1]:
                log.user(f"  > {item[0]}:{unitest['name']}...")
                bininfo_list.append((
                    f"{item[0]}:{unitest['name']}",
                    cmake_unitest_compile(
                        unitest['name'],
                        unitest['prefix'],
                        self._verbose
                    ),
                    unitest['requirements']
                ))
        log.user('- execute unitests...')
        for bininfo in bininfo_list:
            log.user(f"   > {bininfo[0]}...")
            if criterion_unitests_run(bininfo) != 0:
                continue
            gcovr_unitests_run(bininfo)
