"""
core.build - build abstraction
"""
import os
import sys

from core.logger import log
from core.build.bootloader import bootloader_configure
from core.build.kernel import kernel_configure
from core.build.cmake import cmake_build_compile
from core.build.aslr import aslr_generate

from version import (
#    VXGOS_OS_VERSION,
    VXGOS_KERNEL_VERSION,
    VXGOS_BOOTLOADER_VERSION,
)

__all__ = [
    'VxOSBuild',
]

#---
# Public
#---

class VxOSBuild():
    """ performs build abstraction

    The build system of vxGOS is particular
    TODO
    """
    def __new__(cls, *_, **__):
        """ try to acquire boards required file and load external post-scripts
        """
        obj = super().__new__(cls)
        obj._target = os.environ['VXSDK_PKG_TARGET']
        obj._prefix_base = f"{os.path.dirname(__file__)}/../../../../vxgos/"
        obj._prefix_base = os.path.normpath(obj._prefix_base)
        try:
            sys.path.append(f"{obj._prefix_base}/boards/{obj._target}/")
            mod = __import__(
                'generate',
                fromlist=[
                    'generate_aslr_blob',
                    'generate_image'
                ]
            )
            error_base_str = f"{obj._target} : `generate.py` do not exposes"
            if not hasattr(mod, 'generate_aslr_blob'):
                log.emergency(f"{error_base_str} : `generate_aslr_blob()`")
            if not hasattr(mod, 'generate_image'):
                log.emergency(f"{error_base_str} : `generate_image()`")
            obj._postscript = {
                'alsr'  : mod.generate_aslr_blob,
                'image' : mod.generate_image
            }
            sys.path.pop()
        except ImportError as _:
            log.error(f"unable to aquire '{obj._target}' post-script")
        return obj

    def __init__(self, verbose=False):
        """ create build abstraction object """
        self._verbose = verbose
        self._prefix = {
            'build'      : os.environ['VXSDK_PREFIX_BUILD'],
            'bootloader' : f"{self._prefix_base}/bootloader",
            #'kernel'     : f"{self._prefix_base}/kernel",
        }
        self._binlist = {
            'bootloader' : '',
            #'kernel'     : '',
            #'os'         : ''
        }

    #---
    # Attributes
    #---

    @property
    def prefix(self):
        """ return the prefix dictionary """
        return self._prefix

    @property
    def target(self):
        """ return the selected target """
        return self._target

    @property
    def verbose(self):
        """ return if the verbose mode is selected """
        return self._verbose

    @property
    def postscript(self):
        """ return if the verbose mode is selected """
        return self._postscript

    #---
    # Public
    #---

    def configure(self):
        """ compile OS/kernel, bootloader and perform post-build scripts """
        self._binlist = {
            'bootloader': bootloader_configure(self, VXGOS_BOOTLOADER_VERSION),
            #'kernel'    : kernel_configure(self, VXGOS_KERNEL_VERSION),
            #'os'        : '',
        }

    def build(self):
        """ compile OS/kernel, bootloader and perform post-build scripts

        TODO
        """
        binpathlist = {}
        log.user("[+] compiling each OS part...")
        for partname in self._binlist:
            log.user(f"- {partname}...")
            binpathlist[partname] = cmake_build_compile(
                partname,
                f"{self.prefix['build']}/{partname}/",
                self.verbose
            )

        blobpathlist = {}
        log.user("[+] generate ASLR information...")
        for bininfo in binpathlist.items():
            log.user(f"- {bininfo[0]}...")
            blobpathlist[bininfo[0]] = aslr_generate(self, bininfo[1])

        return self.postscript['image'](
            self.prefix['build'],
            blobpathlist['bootloader'],
            '',
            #blobpathlist['kernel'],
            #blobpathlist['os']
        )
