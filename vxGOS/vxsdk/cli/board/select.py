"""
vxsdk.cli.board.select  - select a new target
"""
__all__ = [
    'vxsdk_cli_board_select_entry',
]
from typing import NoReturn
import sys

import click

from vxsdk.core.exception import SDKException
from vxsdk.core.board import board_manager_select
from vxsdk.core.logger import log

#---
# Public
#---

@click.command('select')
@click.argument('board_name')
def vxsdk_cli_board_select_entry(board_name: str) -> NoReturn:
    """ select a new board (and initialise it if needed)

    BOARD_NAME is the board to select
    """
    try:
        board_manager_select(board_name)
        sys.exit(0)
    except SDKException as err:
        log.error(err)
        sys.exit(1)
