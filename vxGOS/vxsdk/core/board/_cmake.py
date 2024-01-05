"""
vxsdk.core.board._cmake     - cmake abstraction
"""
__all__ = [
    'CMakeToolchainInfo',
    'cmake_generate_toolchain',
    'cmake_generate_cmakefile',
    'cmake_build',
]
from typing import List
from pathlib import Path
from dataclasses import dataclass
import subprocess
import re

from vxsdk.core.logger import log

#---
# Internals
#---

_CMAKE_TOOLCHAIN_TEMPLATE = """
# generated CMake toolchain file
# all modification to this file will be automatically removed

set(CMAKE_SYSTEM_NAME Vhex)
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

project({VXSDK_PROJ_NAME} LANGUAGES C ASM)

include_directories(
  {VXSDK_BUILD_INCLUDES}
)
add_compile_options(
  {VXSDK_BUILD_CFLAGS}
)
add_link_options(
  {VXSDK_BUILD_LDFLAGS}
)
set(
  CMAKE_EXE_LINKER_FLAGS
  "${CMAKE_EXE_LINKER_FLAGS} -T {VXSDK_BUILD_LINKER}"
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

add_executable({VXSDK_PROJ_NAME} ${VXSDK_PROJ_SOURCES})
target_link_libraries({VXSDK_PROJ_NAME} {VXSDK_BUILD_LIBS})
""".strip()

def _cmake_file_update(file_pathname: Path, content: str) -> bool:
    """ check if the file need to be recreated
    """
    if file_pathname.exists():
        with open(file_pathname, 'r', encoding='ascii') as cmakefile:
            if cmakefile.read() == content:
                return False
        file_pathname.unlink()
    file_pathname.parent.mkdir(parents=True, exist_ok=True)
    with open(file_pathname, 'x', encoding='ascii') as toolfile:
        toolfile.write(content)
    (file_pathname.parent/'need_reconfig').touch()
    return True

def _cmake_cmd_exec(cmd: str) -> None:
    """ Popen wrapper
    """
    with subprocess.Popen(cmd.split()) as procinfo:
        procinfo.wait()
        if procinfo.returncode != 0:
            log.emergency(f"Unable to execute CMake command '{cmd}'")

#---
# Public
#---

## dataclasses

@dataclass
class CMakeToolchainInfo():
    """ toolchain packed information """
    prefix:     str
    processor:  str

@dataclass
class CMakeFileInfo():
    """ cmakefile packed information """
    includes:   List[str]
    srcs:       List[str]
    linker:     Path
    cflags:     List[str]
    ldflags:    List[str]
    libraries:  List[str]

## functions

def cmake_generate_toolchain(
    prefix_build: Path,
    conf: CMakeToolchainInfo,
) -> None:
    """ generate the toolchain file if needed
    """
    raw = _CMAKE_TOOLCHAIN_TEMPLATE
    raw = re.sub('{VXSDK_TOOLCHAIN_PROCESSOR}', conf.processor, raw)
    raw = re.sub('{VXSDK_TOOLCHAIN_PREFIX}', conf.prefix, raw)
    _cmake_file_update(prefix_build/'toolchain.cmake', raw)

def cmake_generate_cmakefile(
    prefix_build: Path,
    project_name: str,
    conf: CMakeFileInfo,
) -> None:
    """ generate the cmakelist.txt file
    """
    str_linker  = str(conf.linker)
    str_cflags  = '\n  '.join(conf.cflags)
    str_ldflags = '\n  '.join(conf.ldflags)
    str_srcs    = '\n  '.join([str(x) for x in conf.srcs])
    str_hdrs    = '\n  '.join([str(x) for x in conf.includes])
    str_libs    = '\n  '.join([str(x) for x in conf.libraries])
    raw = _CMAKE_TEMPLATE
    raw = re.sub('{VXSDK_PROJ_NAME}',       project_name,   raw)
    raw = re.sub('{VXSDK_BUILD_LINKER}',    str_linker,     raw)
    raw = re.sub('{VXSDK_BUILD_CFLAGS}',    str_cflags,     raw)
    raw = re.sub('{VXSDK_BUILD_LDFLAGS}',   str_ldflags,    raw)
    raw = re.sub('{VXSDK_SOURCE_FILES}',    str_srcs,       raw)
    raw = re.sub('{VXSDK_BUILD_INCLUDES}',  str_hdrs,       raw)
    raw = re.sub('{VXSDK_BUILD_LIBS}',      str_libs,       raw)
    _cmake_file_update(prefix_build/'CMakeLists.txt', raw)

def cmake_build(prefix_cmake: Path, prefix_build: Path) -> None:
    """ perform build operation
    """
    if (prefix_cmake/'need_reconfig').exists():
        _cmake_cmd_exec(
            f"cmake -B {str(prefix_build)} -S {str(prefix_cmake)} "
            f"-DCMAKE_TOOLCHAIN_FILE={str(prefix_cmake/'toolchain.cmake')}"
        )
        (prefix_cmake/'need_reconfig').unlink()
    _cmake_cmd_exec(f"cmake --build {str(prefix_build)}")
