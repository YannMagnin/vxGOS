"""
vxsdk.core.board.manager    - board manager
"""
__all__ = [
    'board_manager_iterate',
    'board_manager_initialise',
]
from dataclasses import dataclass
from typing import Generator

from vxsdk.core.logger import log
from vxsdk.core.board.exception import BoardException
from vxsdk.core.board._bootloader import board_bootloader_initialise
#from vxsdk.core.board._kernel import board_kernel_initialise
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

def board_manager_initialise(board_name: str) -> None:
    """ initialise a new board
    """
    if not (CONFIG_SDK_PREFIX_BOARDS/board_name).exists():
        raise BoardException(f"board '{board_name}' does not exists")
    board_bootloader_initialise(board_name)
#    board_kernel_initialise(board_name)
#    board_os_initialise(board_name)
#    if not board_manager_get_selected():
#        board_manager_select(board_name)
