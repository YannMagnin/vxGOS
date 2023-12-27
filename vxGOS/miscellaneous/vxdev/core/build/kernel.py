"""
vxdev.core.build.kernel - kernel build abstraction
"""
import os
import glob
import toml

from core.logger import log
from core.build.cmake import cmake_build_configure
from core.build.compiles import compiles_fetch

__all__ = [
    'kernel_configure',
]

#---
# Internals
#---

def _fetch_board_config(build, board_cfg_path):
    """ try to find and load the board configuratio file

    This routine will check the TOML boards configuration validity and return
    only the 'config' section which contain most of the information needed to
    fetch all kernel source files

    @return
    > a dictionary with all configuration information
    > give up if an error is detected
    """
    if not os.path.exists(board_cfg_path):
        log.emergency("unable to find the kernel '{build.target}' board file")
    log.debug(f"> board config = {board_cfg_path}")
    board_cfg = toml.load(board_cfg_path)
    if 'meta' not in board_cfg:
        log.emergency(f"{build.target}: missing board 'meta' information")
    if 'config' not in board_cfg:
        log.emergency(f"{build.target}: missing board 'config' information")
    if 'modules' not in board_cfg['config']:
        log.emergency(f"{build.target}: missing board 'modules' information")
    if 'drivers' not in board_cfg['config']:
        log.emergency(f"{build.target}: missing board 'drivers' information")
    return board_cfg['config']

def _fetch_board_sources(build, prefix):
    """ fetch board specific sources files

    Scan the target board source directory (<prefix>/src/) and fetch all C
    and assembly file.

    @return
    > a list of all source file found
    """
    prefix = f"{prefix}/boards/{build.target}/src"
    if not os.path.exists(prefix):
        return glob.glob(f"{prefix}/**/*.[csS]", recursive=True)
    return []

def _fetch_common_sources(_, prefix):
    """ fetch common source file
    """
    return glob.glob(f"{prefix}/src/*.[csS]", recursive=False)

def _fetch_drivers_sources(build, prefix, drvlist):
    """ fetch selected drivers
    """
    sources = []
    for drv in drvlist:
        if not os.path.exists(f"{prefix}/src/drivers/{drv}"):
            log.emergency(f"{build.target}: unable to find driver '{drv}'")
        sources += glob.glob(
            f"{prefix}/src/drivers/{drv}/**/*.[csS]",
            recursive=True
        )
    return sources

def _fetch_modules_sources(build, prefix, modlist):
    """ fetch selected kernel module
    """
    sources = []
    for mod in modlist:
        if not os.path.exists(f"{prefix}/src/modules/{mod}"):
            log.emergency(f"{build.target}: unable to find module '{mod}'")
        sources += glob.glob(
            f"{prefix}/src/modules/{mod}/**/*.[csS]",
            recursive=True
        )
    return sources

#---
# Public
#---

def kernel_configure(build, version):
    """ configure the kernel

    TODO
    """
    prefix = build.prefix['kernel']
    log.debug('fetching kernel files...')
    log.debug(f"> prefix = {prefix}")

    log.debug('- fetching compiles informations...')
    compiles = compiles_fetch(f"{prefix}/boards/{build.target}/compiles.toml")
    if not compiles:
        log.emergency(
            "unable to find kernel's compilation information (compiles.toml)"
        )

    board_cfg = _fetch_board_config(
        build,
        f"{prefix}/boards/{build.target}/board.toml"
    )
    sources  = _fetch_board_sources(build, prefix)
    sources += _fetch_common_sources(build, prefix)
    sources += _fetch_drivers_sources(build, prefix, board_cfg['drivers'])
    sources += _fetch_modules_sources(build, prefix, board_cfg['modules'])
    log.debug("> fetched sources = {sources}")

    prefix = f"{build.prefix['build']}/kernel/"
    include_list = [
        f"{build.prefix['kernel']}/include/",
        f"{build.prefix['kernel']}/boards/{build.target}/include/"
    ]
    return cmake_build_configure(
        prefix,
        {
            'name'    : 'kernel',
            'version' : version,
            'linker'  : f"{build.prefix['kernel']}/kernel.ld",
            'ldflags' : compiles.ldflags,
            'cflags'  : compiles.cflags,
            'include' : include_list,
            'src'     : sources,
            'libs'    : compiles.libs,
            'toolchain_prefix' : compiles.toolchain_prefix,
            'toolchain_proc'   : compiles.toolchain_processor,
        }
    )
