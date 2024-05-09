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
from typing import Optional, Any, cast
from collections.abc import Generator
from dataclasses import dataclass
from pathlib import Path
from copy import deepcopy
import os
import sys
import shutil

from vxsdk.core.logger import log
from vxsdk.core.board.exception import BoardException
from vxsdk.core.board._config import board_config_load
from vxsdk.core.board._bootloader import (
    board_bootloader_initialise,
    board_bootloader_build,
)
from vxsdk.core.board._kernel import (
    board_kernel_initialise,
    board_kernel_build,
)
from vxsdk.core._config import (
    CONFIG_SDK_PREFIX_BOARDS,
    CONFIG_SDK_PREFIX_BUILD,
    CONFIG_SDK_PREFIX_SRCS,
)

#---
# Internals
#---

def _board_manager_load_generator(board: str) -> dict[str,Any]:
    """ load the `board/generator.py`
    """
    try:
        sys.path.append(str(CONFIG_SDK_PREFIX_BOARDS/board))
        mod = __import__(
            name        = 'generator',
            fromlist    = [
                'generate_aslr_blob',
                'generate_final_image',
            ],
        )
        sys.path.pop()
        error_base_str = \
            f"{board} : `vxgos/board/{board}/generate.py` do not exposes"
        if not hasattr(mod, 'generate_aslr_blob'):
            log.emergency(f"{error_base_str} : `generate_aslr_blob()`")
        if not hasattr(mod, 'generate_final_image'):
            log.emergency(f"{error_base_str} : `generate_final_image()`")
        return {
            'alsr'  : mod.generate_aslr_blob,
            'image' : mod.generate_final_image,
        }
    except ImportError:
        log.emergency(
            f"Unable to aquire the `vxgos/board/{board}/generator.py` "
            'script'
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

@dataclass
class BoardSelected():
    """ return board selected information """
    name:           str
    prefix_build:   Path
    prefix_board:   Path

## functions

def board_manager_select_get() -> BoardSelected|None:
    """ return the selected board
    """
    if not (selected := CONFIG_SDK_PREFIX_BUILD/'SELECT').exists():
        return None
    selected = selected.resolve()
    return BoardSelected(
        name            = selected.name,
        prefix_build    = selected,
        prefix_board    = CONFIG_SDK_PREFIX_SRCS/f"boards/{selected.name}",
    )

def board_manager_iterate() -> Generator[BoardIterInfo,None,None]:
    """ iterate over all boards
    """
    for board in CONFIG_SDK_PREFIX_BOARDS.iterdir():
        if not board.is_dir():
            log.warning(f"{str(board)} is not a folder, skipped")
            continue
        board_info = BoardIterInfo(
            is_select   = False,
            is_config   = False,
            name        = board.name,
        )
        board_prefix_build = CONFIG_SDK_PREFIX_BUILD/board.name
        if board_prefix_build.exists():
            board_info.is_config = True
        if (selected := board_manager_select_get()):
            if board_prefix_build == selected.prefix_build:
                board_info.is_select = True
        yield board_info

def board_manager_initialise(board_name: str) -> None:
    """ initialise a new board
    """
    if not (CONFIG_SDK_PREFIX_BOARDS/board_name).exists():
        raise BoardException(f"board '{board_name}' does not exists")
    board_config = board_config_load(
        CONFIG_SDK_PREFIX_SRCS/f"boards/{board_name}/config.toml",
    )
    board_bootloader_initialise(board_name, deepcopy(board_config))
    board_kernel_initialise(board_name, deepcopy(board_config))
    #board_os_initialise(board_name, deepcopy(board_config))

def board_manager_select(board_name: str) -> None:
    """ select a new board
    """
    if not (CONFIG_SDK_PREFIX_BUILD/board_name).exists():
        try:
            board_manager_initialise(board_name)
        except BoardException as err:
            shutil.rmtree(
                path            = CONFIG_SDK_PREFIX_BUILD/board_name,
                ignore_errors   = True,
            )
            raise err
    selected = CONFIG_SDK_PREFIX_BUILD/'SELECT'
    selected.unlink(missing_ok=True)
    selected.symlink_to(CONFIG_SDK_PREFIX_BUILD/board_name)
    log.user(f"Selecting board '{board_name}'")

def board_manager_build(
    project_target: Optional[str],
    enable_verbose: bool,
) -> Path:
    """ build projects
    """
    if not (board := board_manager_select_get()):
        raise BoardException('No board selected or configured, abord')
    if enable_verbose:
        os.environ['VERBOSE'] = "1"
    board_config = board_config_load(board.prefix_board/'config.toml')
    generator = _board_manager_load_generator(board.name)
    if project_target:
        return {
            'bootloader' : board_bootloader_build,
            'kernel'     : board_kernel_build,
            #'os'        : board_os_build,
        }[project_target](board.name, deepcopy(board_config), generator)
    file = (
        board_bootloader_build(
            board.name,
            deepcopy(board_config),
            generator,
        ),
        board_kernel_build(
            board.name,
            deepcopy(board_config),
            generator,
        ),
        None,
    )
    log.user('[+] generate final image...')
    return cast(
        Path,
        generator['image'](
            board.prefix_build,
            file[0],
            file[1],
            file[2],
        ),
    )
