"""
core.unitests.cmake - CMake abstraction for unitests
"""
import os
import subprocess
import re

from core.logger import log

__all__ = [
    'cmake_unitest_configure',
    'cmake_unitest_compile',
]

#---
# Internals
#---

_CMAKE_TEMPLATE = """
cmake_minimum_required(VERSION 3.15)

#---
# project-specific information
#---

project({VXDEV_PROJ_NAME} VERSION 0.1.0 LANGUAGES C ASM)

include_directories(
  {VXDEV_BUILD_INCLUDES}
)
add_compile_options(
  -Wall
  -Wextra
  -std=c2x
  -g3
  --coverage
  -fsanitize=address
)
add_link_options(
  --coverage
  -lasan
  -lcriterion
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
target_link_libraries({VXDEV_PROJ_NAME})
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

def _cmake_unitest_generate_template(prefix, proj):
    """ Generate a common CMake file

    @args
    > proj  (dict) - project information
    > prefix (str) - build prefix
    """
    cmakefile_prefix   = f"{prefix}"
    cmakefile_pathname = f"{cmakefile_prefix}/CMakeLists.txt"

    proj['src']     = '\n  '.join(proj['src'])
    proj['include'] = '\n  '.join(proj['include'])

    content = _CMAKE_TEMPLATE
    content = re.sub('{VXDEV_PROJ_NAME}',      proj['name'],    content)
    content = re.sub('{VXDEV_SOURCE_FILES}',   proj['src'],     content)
    content = re.sub('{VXDEV_BUILD_INCLUDES}', proj['include'], content)

    if not _cmakefile_need_update(cmakefile_pathname, content):
        return False
    if not os.path.exists(cmakefile_prefix):
        os.makedirs(cmakefile_prefix)

    log.debug(f"generate cmakefile at '{cmakefile_prefix}'...")
    with open(cmakefile_pathname, 'x', encoding='ascii') as cmakefile:
        cmakefile.write(content)
    return True

#---
# Pulbic
#---

def cmake_unitest_configure(prefix, proj):
    """ Abstract cmake configuration

    @args
    > name   (str) - project name
    > prefix (str) - build prefix
    """
    if not _cmake_unitest_generate_template(prefix, proj):
        return prefix
    shell_cmd = f"cmake -B {prefix} -S {prefix}"
    if subprocess.run(shell_cmd.split(), check=False).returncode != 0:
        log.emergency(f"{proj['name']}: unable to configure the projet, abord")
    return prefix

def cmake_unitest_compile(name, prefix, verbose):
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
