"""
vxsdk.core.board.manager    - board manager
"""
__all__ = [
    'board_manager_iterate',
]
from dataclasses import dataclass
from typing import Generator

from vxsdk.core.logger import log
from vxsdk.core._config import CONFIG_SDK_PREFIX_BOARDS

#---
# Public
#---

@dataclass
class BoardIterInfo():
    """ board information """
    is_select:  bool
    is_config:  bool
    name:       str

def board_manager_iterate() -> Generator[BoardIterInfo,None,None]:
    """ iterate over all boards """
    for board in CONFIG_SDK_PREFIX_BOARDS.iterdir():
        if not board.is_dir():
            log.warn(f"{str(board)} is not a folder, skipped")
            continue
        yield BoardIterInfo(
            is_select   = False,
            is_config   = False,
            name        = board.name,
        )
