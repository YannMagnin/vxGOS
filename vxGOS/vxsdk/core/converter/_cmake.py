"""
vxsdk.core.converter._font._cmake   - cmake abstraction
"""
__all__ = [
    'converter_cmake_build',
]
from typing import Dict, Any, List
from pathlib import Path

from vxsdk.core.utils import utils_cmake_build

#---
# Internals
#---

CONVERTER_GENERIC_CMAKE_TEMPLATE = """
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
# Public
#---

def converter_cmake_build(
    prefix_build:       Path,
    prefix_include:     Path,
    compile_conf:       Dict[str,Any],
    asset_outfile_list: List[Path],
) -> Path:
    """ build the assets library using cmake and return the library
    """
    utils_cmake_build(
        project_name    = 'assets',
        prefix_build    = prefix_build,
        compile_conf    = compile_conf,
        compile_files   = {
            'includes'  : [prefix_include],
            'srcs'      : asset_outfile_list,
        },
        linker_path         = None,
        cmakelist_template  = CONVERTER_GENERIC_CMAKE_TEMPLATE,
    )
    return prefix_build/'_build/libassets.a'
