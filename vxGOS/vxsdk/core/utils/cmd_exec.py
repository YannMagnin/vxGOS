"""
vxsdk.core._utils.cmd_exec  - command execute helper
"""
__all__ = [
    'utils_cmd_exec',
]
import subprocess
import shutil

from vxsdk.core.logger import log

#---
# Public
#---

def utils_cmd_exec(cmd: str) -> None:
    """ simple Popen wrapper
    """
    if not shutil.which((cmd_list := cmd.split())[0]):
        log.emergency(f"Missing external binary '{cmd_list[0]}'")
    with subprocess.Popen(cmd_list) as procinfo:
        procinfo.wait()
        if procinfo.returncode != 0:
            log.emergency(f"Unable to execute shell command '{cmd}'")
