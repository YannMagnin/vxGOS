"""
vxsdk.core.converter._font._cmake   - cmake abstraction
"""
__all__ = [
    'converter_cmake_build',
]
from typing import Dict, Any, List, Optional
from pathlib import Path
import shutil
import re

from vxsdk.core._utils import utils_file_update, utils_cmd_exec

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

_CMAKE_TEMPLATE = """
cmake_minimum_required(VERSION 3.15)

#---
# project-specific information
#---

project(assets LANGUAGES C ASM)

include_directories(
  {VXSDK_BUILD_INCLUDES}
)
add_compile_options(
  {VXSDK_BUILD_CFLAGS}
)

#---
# source files listing
#---

set(
  VXSDK_PROJ_SOURCES
  {VXSDK_SOURCE_FILES}
)

#---
# commit projet
#---

add_library(assets ${VXSDK_PROJ_SOURCES})
""".strip()


#---
# Internals
#---

def _converter_cmake_generate_toolchain(
    prefix_build: Path,
    compile_conf: Dict[str,Any],
) -> bool:
    """ generate the toolchain file if needed
    """
    conf_processor  = compile_conf['toolchain']['processor']
    conf_toolchain  = compile_conf['toolchain']['prefix']
    raw = _CMAKE_TOOLCHAIN_TEMPLATE
    raw = re.sub('{VXSDK_TOOLCHAIN_PROCESSOR}', conf_processor, raw)
    raw = re.sub('{VXSDK_TOOLCHAIN_PREFIX}',    conf_toolchain, raw)
    return utils_file_update(prefix_build/'toolchain.cmake', raw)

def _converter_cmake_generate_cmakelist(
    prefix_build:   Path,
    prefix_include: Path,
    source_list:    List[Path],
    compile_conf:   Dict[str,Any],
) -> bool:
    """ generate the cmakelist.txt file
    """
    str_hdrs    = str(prefix_include)
    str_cflags  = '\n  '.join(compile_conf['toolchain']['cflags'])
    str_ldflags = '\n  '.join(compile_conf['toolchain']['ldflags'])
    str_srcs    = '\n  '.join([str(x) for x in source_list])
    raw = _CMAKE_TEMPLATE
    raw = re.sub('{VXSDK_BUILD_CFLAGS}',    str_cflags,     raw)
    raw = re.sub('{VXSDK_BUILD_LDFLAGS}',   str_ldflags,    raw)
    raw = re.sub('{VXSDK_SOURCE_FILES}',    str_srcs,       raw)
    raw = re.sub('{VXSDK_BUILD_INCLUDES}',  str_hdrs,       raw)
    return utils_file_update(prefix_build/'CMakeLists.txt', raw)

#---
# Public
#---

def converter_cmake_build(
    prefix_build:       Path,
    prefix_include:     Path,
    compconf:           Dict[str,Any],
    asset_outfile_list: List[Path],
    need_build:         bool
) -> Optional[Path]:
    """ build the assets library using cmake and return the library
    """
    new_toolchain = _converter_cmake_generate_toolchain(
        prefix_build,
        compconf,
    )
    new_cmakelist = _converter_cmake_generate_cmakelist(
        prefix_build,
        prefix_include,
        asset_outfile_list,
        compconf,
    )
    if new_toolchain or new_cmakelist:
        utils_cmd_exec(
            f"cmake -B {str(prefix_build/'_build')} "
            f"-S {str(prefix_build)} "
            f"-DCMAKE_TOOLCHAIN_FILE={str(prefix_build/'toolchain.cmake')}"
        )
        need_build = True
    if need_build:
        utils_cmd_exec(f"cmake --build {str(prefix_build/'_build')}")
        shutil.copy(
            prefix_build/'_build/libassets.a',
            prefix_build/'libassets.a',
        )
    return prefix_build/'libassets.a'
