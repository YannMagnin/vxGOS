"""
vxsdk.core.board._aslr  - ASLR abstraction
"""
__all__ = [
    'board_aslr_generate',
]
from typing import Dict, Any, List, cast
from pathlib import Path
import subprocess

from vxsdk.core.logger import log

#---
# Internals
#---

def _board_aslr_get_symtab(binpath: Path) -> List[Any]:
    """ fetch all section relocation information
    """
    ret = subprocess.run(
        f"readelf --relocs {str(binpath)}".split(),
        capture_output  = True,
        check           = False,
        text            = True,
    )
    if ret.returncode != 0:
        log.error(ret.stderr)
    symtab = []
    for i, line in enumerate(ret.stdout.splitlines()):
        if i < 3:
            continue
        if len(sinfo := line.split()) != 7:
            continue
        log.debug(f"[{i}] {sinfo}")
        symtab.append(sinfo)
    return symtab

def _board_aslr_get_sectab(binpath: Path) -> List[Any]:
    """ fetch all section information (address, name, ...)
    """
    ret = subprocess.run(
        f"readelf --sections --wide {str(binpath)}".split(),
        capture_output  = True,
        check           = False,
        text            = True,
    )
    if ret.returncode != 0:
        log.error(ret.stderr)
    sectab = []
    for i, line in enumerate(ret.stdout.splitlines()):
        if line.find('  [') != 0:
            continue
        secinfo = line[7:].split()
        log.debug(f"[{i}] {secinfo[0]}")
        sectab.append(secinfo)
    return sectab

#---
# Public
#---

# Allowing cathcing general exception because we cannot easily known the
# potential exception that will be raised in the extern script
# pylint: disable=locally-disabled,W0718
def board_aslr_generate(
    project_name:   str,
    prefix_build:   Path,
    elf_file:       Path,
    generator:      Dict[str,Any],
) -> Path:
    """ generate the ALSR blob
    """
    try:
        return cast(
            Path,
            generator['alsr'](
                project_name    = project_name,
                prefix_build    = prefix_build,
                elf_file        = elf_file,
                symtab          = _board_aslr_get_symtab(elf_file),
                sectab          = _board_aslr_get_sectab(elf_file),
            ),
        )
    except Exception as err:
        log.emergency(f"External script has crashed -> {err}")
