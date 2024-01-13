"""
vxsdk.core._utils.cmd_exec  - command execute helper
"""
__all__ = [
    'utils_cmd_exec',
]
import subprocess

from vxsdk.core.logger import log

#---
# Public
#---

def utils_cmd_exec(cmd: str) -> None:
    """ simple Popen wrapper
    """
    with subprocess.Popen(cmd.split()) as procinfo:
        procinfo.wait()
        if procinfo.returncode != 0:
            log.emergency(f"Unable to execute shell command '{cmd}'")
