"""
CMake abstraction
"""
import os
import subprocess
import re

from core.logger import log

__all__ = [
    'cmake_build_configure',
    'cmake_build_compile',
]

#---
# Internals
#---

_CMAKE_TOOLCHAIN_TEMPLATE = """
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR {VXDEV_TOOLCHAIN_PROCESSOR})

set(CMAKE_C_COMPILER {VXDEV_TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER {VXDEV_TOOLCHAIN_PREFIX}g++)

set(CMAKE_C_FLAGS_INIT "")
set(CMAKE_CXX_FLAGS_INIT "")

# required to avoid CMake compiler check fails
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

project({VXDEV_PROJ_NAME} VERSION {VXDEV_PROJ_VERSION} LANGUAGES C ASM)

include_directories(
  {VXDEV_BUILD_INCLUDES}
)
add_compile_options(
  {VXDEV_BUILD_CFLAGS}
)
add_link_options(
  {VXDEV_BUILD_LDFLAGS}
)
set(
  CMAKE_EXE_LINKER_FLAGS
  "${CMAKE_EXE_LINKER_FLAGS} -T {VXDEV_BUILD_LINKER}"
)

#---
# source files listing
#---

set(
  VXDEV_PROJ_SOURCES
  {VXDEV_SOURCE_FILES}
)

#---
# commit projet
#---

add_executable({VXDEV_PROJ_NAME} ${VXDEV_PROJ_SOURCES})
target_link_libraries({VXDEV_PROJ_NAME} {VXDEV_BUILD_LIBS})
""".strip()

def _cmakefile_need_update(cmakefile_pathname, content):
    """ check if the current CMakeLists.txt need to be recreated
    """
    if not os.path.exists(cmakefile_pathname):
        return True
    with open(cmakefile_pathname, 'r', encoding='ascii') as cmakefile:
        if cmakefile.read() != content:
            os.remove(cmakefile_pathname)
            return True
    return False

def _cmake_build_generate_template(prefix, proj):
    """ Generate a common CMake file

    @args
    > proj  (dict) - project information
    > prefix (str) - build prefix
    """
    cmakefile_prefix   = f"{prefix}"
    cmakefile_pathname = f"{cmakefile_prefix}/CMakeLists.txt"

    proj['src']     = '\n  '.join(proj['src'])
    proj['include'] = '\n  '.join(proj['include'])
    proj['ldflags'] = '\n  '.join(proj['ldflags'])
    proj['cflags']  = '\n  '.join(proj['cflags'])
    proj['libs']    = '\n  '.join(proj['libs'])

    content = _CMAKE_TEMPLATE
    content = re.sub('{VXDEV_PROJ_NAME}',      proj['name'],    content)
    content = re.sub('{VXDEV_PROJ_VERSION}',   proj['version'], content)
    content = re.sub('{VXDEV_BUILD_CFLAGS}',   proj['cflags'],  content)
    content = re.sub('{VXDEV_BUILD_LDFLAGS}',  proj['ldflags'], content)
    content = re.sub('{VXDEV_BUILD_LINKER}',   proj['linker'],  content)
    content = re.sub('{VXDEV_SOURCE_FILES}',   proj['src'],     content)
    content = re.sub('{VXDEV_BUILD_INCLUDES}', proj['include'], content)
    content = re.sub('{VXDEV_BUILD_LIBS}',     proj['libs'],    content)

    if not _cmakefile_need_update(cmakefile_pathname, content):
        return False
    if not os.path.exists(cmakefile_prefix):
        os.makedirs(cmakefile_prefix)

    log.debug(f"generate cmakefile at '{cmakefile_prefix}'...")
    with open(cmakefile_pathname, 'x', encoding='ascii') as cmakefile:
        cmakefile.write(content)
    return True

def _cmake_build_generate_toolchain(prefix, proj):
    """ generate the toolchain file
    """
    toolchain_proc = proj['toolchain_proc']
    toolchain_prefix = proj['toolchain_prefix']
    cmakefile_pathname = f"{prefix}/toolchain.cmake"

    content = _CMAKE_TOOLCHAIN_TEMPLATE
    content = re.sub('{VXDEV_TOOLCHAIN_PROCESSOR}', toolchain_proc, content)
    content = re.sub('{VXDEV_TOOLCHAIN_PREFIX}', toolchain_prefix, content)

    if not _cmakefile_need_update(cmakefile_pathname, content):
        return False
    if not os.path.exists(prefix):
        os.makedirs(prefix)

    log.debug(f"generate cmakefile at '{prefix}'...")
    with open(cmakefile_pathname, 'x', encoding='ascii') as cmakefile:
        cmakefile.write(content)
    return True

#---
# Pulbic
#---

def cmake_build_configure(prefix, proj):
    """ Abstract cmake configuration

    @args
    > name   (str) - project name
    > prefix (str) - build prefix
    """
    toolchain_flag = ''
    check = _cmake_build_generate_toolchain(prefix, proj)
    if proj['toolchain_prefix']:
        check += _cmake_build_generate_template(prefix, proj)
        toolchain_flag = f"-DCMAKE_TOOLCHAIN_FILE={prefix}/toolchain.cmake"
    if check == 0:
        return
    shell_cmd = f"cmake {toolchain_flag} -B {prefix} -S {prefix}"
    if subprocess.run(shell_cmd.split(), check=False).returncode != 0:
        log.emergency(f"{proj['name']}: unable to configure the projet, abord")

def cmake_build_compile(name, prefix, verbose):
    """ Abstract cmake configuration

    @args
    > name     (str) - project name
    > prefix   (str) - build prefix
    > verbose (bool) - build verbose

    @return
    > the generated binary pathname
    """
    shell_cmd = f"cmake --build {prefix}"
    if verbose:
        shell_cmd += ' --verbose'
    if subprocess.run(shell_cmd.split(), check=False).returncode != 0:
        log.emergency(f"{name}: unable to configure the projet, abord")
    return f"{prefix}/{name}"
