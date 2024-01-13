"""
vxsdk.core.board._bootloader    - bootloader abstraction
"""
__all__ = [
    'board_bootloader_initialise',
    'board_bootloader_build',
]
from typing import Dict, Any
from pathlib import Path

from vxsdk.core.logger import log
from vxsdk.core.board._aslr import board_aslr_generate
from vxsdk.core.converter.manager import converter_manager_generate
from vxsdk.core.utils import utils_compile_conf_load
from vxsdk.core.board._cmake import board_cmake_build
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
    utils_compile_conf_load(
        CONFIG_SDK_PREFIX_SRCS/f"boards/{board_name}/compiles.toml",
    )
    prefix_build = CONFIG_SDK_PREFIX_BUILD/f"{board_name}/bootloader"
    prefix_build.mkdir(parents=True, exist_ok=True)

def board_bootloader_build(
    board_name: str,
    generator:  Dict[str,Any],
) -> Path:
    """ generate the ELF bootloader information
    """
    log.user('[+] preliminary checks...')
    prefix_build = CONFIG_SDK_PREFIX_BUILD/f"{board_name}/bootloader"
    prefix_src   = CONFIG_SDK_PREFIX_SRCS/'bootloader'
    compile_conf = utils_compile_conf_load(
        CONFIG_SDK_PREFIX_SRCS/f"boards/{board_name}/compiles.toml",
    )
    compile_file = _bootloader_find_srcs(prefix_src, board_name)
    log.user('[+] building assets...')
    asset_library = converter_manager_generate(
        CONFIG_SDK_PREFIX_SRCS/'bootloader/assets/',
        CONFIG_SDK_PREFIX_SRCS/'bootloader/include/',
        prefix_build/'_assets/',
        'bootloader',
        compile_conf,
    )
    compile_conf['toolchain']['libraries'].insert(0, '-lassets')
    compile_conf['toolchain']['ldflags'].append(
        f"-L{str(asset_library.parent)}",
    )
    log.user('[+] building bootloader...')
    bootloader_elf = board_cmake_build(
        'bootloader',
        prefix_build,
        compile_conf,
        compile_file,
        prefix_src/'bootloader.ld',
    )
    log.user('[+] construct the bootloader ALSR blob...')
    return board_aslr_generate(
        'bootloader',
        prefix_build,
        bootloader_elf,
        generator,
    )
