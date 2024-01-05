"""
vxsdk.core.board._bootloader    - bootloader abstraction
"""
__all__ = [
    'board_bootloader_initialise',
    'board_bootloader_build',
]
from typing import Dict, Any
from pathlib import Path

from vxsdk.core.board._common import (
    board_common_initialise,
    board_common_load_compile_conf,
    board_common_build,
)
from vxsdk.core._config import (
    CONFIG_SDK_PREFIX_BUILD,
    CONFIG_SDK_PREFIX_SRCS,
)

#---
# Internals
#---

def _bootloader_find_srcs(prefix: Path, board_name: str) -> Dict[str,Any]:
    """ find all files needed to generate the cmakefile
    """
    return {
        'includes'   : (
            prefix/f"boards/{board_name}/include",
            prefix/'include',
        ),
        'srcs'      : \
              list((prefix/'src').rglob('*.[csS]'))
            + list((prefix/f"boards/{board_name}/src").rglob('*.[csS]')),
    }

#---
# Public
#---

def board_bootloader_initialise(board_name: str) -> None:
    """ initialise the build information needed for the bootloader
    """
    prefix_src   = CONFIG_SDK_PREFIX_SRCS/'bootloader'
    compile_conf = board_common_load_compile_conf(board_name)
    compile_file = _bootloader_find_srcs(prefix_src, board_name)
    board_common_initialise(
        'bootloader',
        CONFIG_SDK_PREFIX_BUILD/f"{board_name}/bootloader",
        prefix_src/'bootloader.ld',
        compile_conf,
        compile_file,
    )

def board_bootloader_build(prefix_build: Path) -> Path:
    """ generate the ELF bootloader information
    """
    return board_common_build(
        'bootloader',
        prefix_build/'bootloader',
        prefix_build/'bootloader/_build/',
    )
