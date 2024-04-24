"""
vxsdk.core.board._cmake     - cmake abstraction
"""
__all__ = [
    'board_cmake_build',
]
from typing import Any
from pathlib import Path

from vxsdk.core.utils import utils_cmake_build

#---
# Internals
#---

_BOARD_GENERIC_CMAKE_TEMPLATE = """
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

#---
# Public
#---

def board_cmake_build(
    project_name:   str,
    prefix_build:   Path,
    compile_conf:   dict[str,Any],
    compile_files:  dict[str,Any],
    linker_path:    Path,
) -> Path:
    """ perform build operation
    """
    utils_cmake_build(
        project_name,
        prefix_build,
        compile_conf,
        compile_files,
        linker_path,
        _BOARD_GENERIC_CMAKE_TEMPLATE,
    )
    return prefix_build/f"_build/{project_name}"
