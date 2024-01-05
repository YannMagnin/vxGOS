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

#---
# Public
#---

@click.command('select')
@click.option(
    '-t', '--target', 'board_target',
    required    = True,
    metavar     = 'BOARD_NAME',
    help        = 'board to initialise',
)
def vxsdk_cli_board_select_entry(board_target: str) -> NoReturn:
    """ select a new board
    """
    try:
        board_manager_select(board_target)
        sys.exit(0)
    except SDKException as err:
        print(err, file=sys.stderr)
        sys.exit(1)
