"""
vxsdk.cli.board.init    - initialise a new board
"""
__all__ = [
    'vxsdk_cli_board_init_entry',
]
import sys

import click

from vxsdk.core.exception import SDKException
from vxsdk.core.board import board_manager_initialise

#---
# Public
#---

@click.command('init')
@click.option(
    '-t', '--target', 'board_target',
    required    = True,
    metavar     = 'BOARD_NAME',
    help        = 'board to initialise',
)
def vxsdk_cli_board_init_entry(board_target: str) -> None:
    """ initialise a new board
    """
    try:
        board_manager_initialise(board_target)
        sys.exit(0)
    except SDKException as err:
        print(err, file=sys.stderr)
        sys.exit(1)
