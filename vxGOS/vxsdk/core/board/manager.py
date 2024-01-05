"""
vxsdk.core.board.manager    - board manager
"""
__all__ = [
    'board_manager_select',
    'board_manager_select_get',
    'board_manager_iterate',
    'board_manager_initialise',
    'board_manager_build',
]
from typing import Generator, Optional
from dataclasses import dataclass
from pathlib import Path
import os

from vxsdk.core.logger import log
from vxsdk.core.board.exception import BoardException
from vxsdk.core.board._bootloader import (
    board_bootloader_initialise,
    board_bootloader_build,
)
#from vxsdk.core.board._kernel import board_kernel_initialise
from vxsdk.core._config import (
    CONFIG_SDK_PREFIX_BOARDS,
    CONFIG_SDK_PREFIX_BUILD,
)

#---
# Public
#---

## dataclasses

@dataclass
class BoardIterInfo():
    """ board information """
    is_select:  bool
    is_config:  bool
    name:       str

## functions

def board_manager_select(board_name: str) -> None:
    """ select a new board
    """
    if not (CONFIG_SDK_PREFIX_BUILD/board_name).exists():
        raise BoardException(
            f"Target board '{board_name}' is not initialised"
        )
    selected = CONFIG_SDK_PREFIX_BUILD/'SELECT'
    selected.unlink(missing_ok=True)
    selected.symlink_to(CONFIG_SDK_PREFIX_BUILD/board_name)
    print(f"Selecting board '{board_name}'")

def board_manager_select_get() -> Path|None:
    """ return the selected board
    """
    if not (selected := CONFIG_SDK_PREFIX_BUILD/'SELECT').exists():
        return None
    return selected.resolve()

def board_manager_iterate() -> Generator[BoardIterInfo,None,None]:
    """ iterate over all boards
    """
    for board in CONFIG_SDK_PREFIX_BOARDS.iterdir():
        if not board.is_dir():
            log.warn(f"{str(board)} is not a folder, skipped")
            continue
        board_info = BoardIterInfo(
            is_select   = False,
            is_config   = False,
            name        = board.name,
        )
        conf_board_prefix = CONFIG_SDK_PREFIX_BUILD/board.name
        if conf_board_prefix.exists():
            board_info.is_config = True
        if conf_board_prefix == board_manager_select_get():
            board_info.is_select = True
        yield board_info

def board_manager_initialise(board_name: str) -> None:
    """ initialise a new board
    """
    if not (CONFIG_SDK_PREFIX_BOARDS/board_name).exists():
        raise BoardException(f"board '{board_name}' does not exists")
    board_bootloader_initialise(board_name)
#    board_kernel_initialise(board_name)
#    board_os_initialise(board_name)
    board_manager_select(board_name)

def board_manager_build(
    project_target: Optional[str],
    enable_verbose: bool,
) -> None:
    """ build projects
    """
    if not (selected := board_manager_select_get()):
        raise BoardException('No board selected or configured, abord')
    if enable_verbose:
        os.environ['VERBOSE'] = "1"
    if project_target:
        {
            'bootloader' : board_bootloader_build,
            #'kernel'    : board_kernel_build,
            #'os'        : board_os_build,
        }[project_target](selected)
        return
    output_info = {
        'bootloader' : board_bootloader_build(selected),
#        'kernel'     : board_kernel_build(selected),
#        'os'         : board_os_build(selected),
    }
    print(output_info)
    # (todo) : generate final image
