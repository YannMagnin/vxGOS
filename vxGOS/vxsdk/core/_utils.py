"""
vxsdk.core._utils   - various helper
"""
__all__ = [
    'utils_file_update',
    'utils_cmd_exec',
]
from pathlib import Path
import subprocess

from vxsdk.core.logger import log

#---
# Public
#---

def utils_file_update(file_pathname: Path, content: str) -> bool:
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
    return True

def utils_cmd_exec(cmd: str) -> None:
    """ simple Popen wrapper
    """
    with subprocess.Popen(cmd.split()) as procinfo:
        procinfo.wait()
        if procinfo.returncode != 0:
            log.emergency(f"Unable to execute CMake command '{cmd}'")
