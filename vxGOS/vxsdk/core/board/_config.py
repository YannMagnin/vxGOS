
"""
vxsdk.core.board._config    - `config.toml` helper
"""
__all__ = [
    'board_config_load',
]
from typing import Any
from pathlib import Path

import toml

from vxsdk.core.logger import log

#---
# Public
#---

def board_config_load(compile_conf_path: Path) -> dict[str,Any]:
    """ loading and aggressive check for compiles.toml file
    """
    if not compile_conf_path.exists():
        log.emergency('Unable to find the compiles.toml file')
    try:
        compile_conf = toml.load(compile_conf_path)
        if 'toolchain' not in compile_conf:
            log.emergency('Missing toolchain information')
        if 'prefix' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain prefix information')
        if 'processor' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain processor information')
        if 'endianness' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain processor information')
        if 'cflags' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain cflags information')
        if 'ldflags' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain ldflags information')
        if 'libraries' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain libraries information')
        return compile_conf
    except toml.TomlDecodeError as err:
        log.emergency(err)
