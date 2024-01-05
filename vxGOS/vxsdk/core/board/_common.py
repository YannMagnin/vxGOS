"""
vxsdk.board._common     - common handling (kernel,OS,bootloader)
"""
__all__ = [
    'board_common_load_compile_conf',
    'board_common_initialise',
    'board_common_build',
]
from typing import Dict, Any
from pathlib import Path
import shutil

import toml

from vxsdk.core.logger import log
from vxsdk.core._config import CONFIG_SDK_PREFIX_SRCS
from vxsdk.core.board._cmake import (
    cmake_generate_toolchain,
    cmake_generate_cmakefile,
    cmake_build,
    CMakeToolchainInfo,
    CMakeFileInfo,
)

#---
# Public
#---

def board_common_load_compile_conf(board_name: str) -> Dict[str,Any]:
    """ load the `compile.toml` stored in board directory
    """
    comp_path = CONFIG_SDK_PREFIX_SRCS/f"boards/{board_name}/compiles.toml"
    if not (comp_path).exists():
        log.emergency('Unable to find the compiles.toml file')
    try:
        compile_conf = toml.load(comp_path)
        if 'toolchain' not in compile_conf:
            log.emergency('Missing toolchain information')
        if 'prefix' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain prefix information')
        if 'processor' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain processor information')
        if 'cflags' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain cflags information')
        if 'ldflags' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain ldflags information')
        if 'libraries' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain endianness information')
        return compile_conf
    except toml.TomlDecodeError as err:
        log.emergency(err)

def board_common_initialise(
    project_name:   str,
    prefix_build:   Path,
    linker_path:    Path,
    compile_conf:   Dict[str,Any],
    compile_file:   Dict[str,Any],
) -> None:
    """ initialise part of the project (bootloader, kernel, OS)
    """
    cmake_generate_toolchain(
        prefix_build,
        CMakeToolchainInfo(
            prefix      = compile_conf['toolchain']['prefix'],
            processor   = compile_conf['toolchain']['processor'],
        ),
    )
    cmake_generate_cmakefile(
        prefix_build,
        project_name,
        CMakeFileInfo(
            includes    = compile_file['includes'],
            srcs        = compile_file['srcs'],
            linker      = linker_path,
            cflags      = compile_conf['toolchain']['cflags'],
            ldflags     = compile_conf['toolchain']['ldflags'],
            libraries   = compile_conf['toolchain']['libraries'],
        ),
    )

def board_common_build(
    project_name: str,
    prefix_cmake: Path,
    prefix_build: Path,
) -> Path:
    """ build the project and isolate the ELF file
    """
    cmake_build(prefix_cmake, prefix_build)
    shutil.copy(prefix_build/project_name, prefix_cmake/project_name)
    return prefix_build/project_name
