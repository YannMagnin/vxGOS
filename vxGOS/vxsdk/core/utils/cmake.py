"""
vxsdk.core.board._cmake     - cmake abstraction
"""
__all__ = [
    'utils_cmake_build',
]
from typing import List, Dict, Any, Optional
from pathlib import Path
from dataclasses import dataclass
import re

from vxsdk.core.utils import utils_file_update, utils_cmd_exec

#---
# Internals
#---

_CMAKE_TOOLCHAIN_TEMPLATE = """
# generated CMake toolchain file
# all modification to this file will be automatically removed

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR {VXSDK_TOOLCHAIN_PROCESSOR})

set(CMAKE_C_COMPILER {VXSDK_TOOLCHAIN_PREFIX}gcc)

set(CMAKE_C_FLAGS_INIT "")

add_compile_options(-nostdlib)
add_link_options(-nostdlib)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
""".strip()

#---
# Internals
#---

## dataclasses

@dataclass
class _CMakeToolchainInfo():
    """ toolchain packed information """
    prefix:     str
    processor:  str

@dataclass
class _CMakelistInfo():
    """ cmakefile packed information """
    includes:   List[str]
    srcs:       List[str]
    linker:     Optional[Path]
    cflags:     List[str]
    ldflags:    List[str]
    libraries:  List[str]

## functions

def _board_cmake_generate_toolchain(
    prefix_build: Path,
    conf: _CMakeToolchainInfo,
) -> bool:
    """ generate the toolchain file if needed
    """
    raw = _CMAKE_TOOLCHAIN_TEMPLATE
    raw = re.sub('{VXSDK_TOOLCHAIN_PROCESSOR}', conf.processor, raw)
    raw = re.sub('{VXSDK_TOOLCHAIN_PREFIX}', conf.prefix, raw)
    return utils_file_update(prefix_build/'toolchain.cmake', raw)

def _board_cmake_generate_cmakelist(
    prefix_build:       Path,
    project_name:       str,
    cmakelist_template: str,
    config:             _CMakelistInfo,
) -> bool:
    """ generate the cmakelist.txt file
    """
    str_linker  = str(config.linker)
    str_cflags  = '\n  '.join(config.cflags)
    str_ldflags = '\n  '.join(config.ldflags)
    str_srcs    = '\n  '.join([str(x) for x in config.srcs])
    str_hdrs    = '\n  '.join([str(x) for x in config.includes])
    str_libs    = '\n  '.join([str(x) for x in config.libraries])
    raw = cmakelist_template
    raw = re.sub('{VXSDK_PROJ_NAME}',       project_name,   raw)
    raw = re.sub('{VXSDK_BUILD_LINKER}',    str_linker,     raw)
    raw = re.sub('{VXSDK_BUILD_CFLAGS}',    str_cflags,     raw)
    raw = re.sub('{VXSDK_BUILD_LDFLAGS}',   str_ldflags,    raw)
    raw = re.sub('{VXSDK_SOURCE_FILES}',    str_srcs,       raw)
    raw = re.sub('{VXSDK_BUILD_INCLUDES}',  str_hdrs,       raw)
    raw = re.sub('{VXSDK_BUILD_LIBS}',      str_libs,       raw)
    return utils_file_update(prefix_build/'CMakeLists.txt', raw)

#---
# Public
#---

# allow more than 5 parameters to avoid boilerplate with cmake abstraction
# pylint: disable=locally-disabled,R0913
def utils_cmake_build(
    project_name:       str,
    prefix_build:       Path,
    compile_conf:       Dict[str,Any],
    compile_files:      Dict[str,Any],
    linker_path:        Optional[Path],
    cmakelist_template: str
) -> None:
    """ perform build operation
    """
    new_toolchain = _board_cmake_generate_toolchain(
        prefix_build,
        _CMakeToolchainInfo(
            prefix      = compile_conf['toolchain']['prefix'],
            processor   = compile_conf['toolchain']['processor'],
        ),
    )
    new_cmakelist = _board_cmake_generate_cmakelist(
        prefix_build,
        project_name,
        cmakelist_template,
        _CMakelistInfo(
            includes    = compile_files['includes'],
            srcs        = compile_files['srcs'],
            linker      = linker_path,
            cflags      = compile_conf['toolchain']['cflags'],
            ldflags     = compile_conf['toolchain']['ldflags'],
            libraries   = compile_conf['toolchain']['libraries'],
        ),
    )
    if new_toolchain or new_cmakelist:
        utils_cmd_exec(
            f"cmake -B {str(prefix_build/'_build')} "
            f"-S {str(prefix_build)} "
            f"-DCMAKE_TOOLCHAIN_FILE={str(prefix_build/'toolchain.cmake')}"
        )
    utils_cmd_exec(f"cmake --build {str(prefix_build/'_build')}")
