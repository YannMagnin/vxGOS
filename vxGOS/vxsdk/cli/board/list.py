"""
vxsdk.cli.board.list    - list all available board
"""
__all__ = [
    'vxsdk_cli_board_list_entry',
]
from typing import NoReturn
import sys

import click

from vxsdk.core.exception import SDKException
from vxsdk.core.board import board_manager_iterate
from vxsdk.core.logger import log

#---
# Public
#---

@click.command('list')
def vxsdk_cli_board_list_entry() -> NoReturn:
    """ list all available boards
    """
    try:
        for binfo in board_manager_iterate():
            select = 'S' if binfo.is_select else '-'
            config = 'C' if binfo.is_config else '-'
            log.user(f"{select}{config} {binfo.name:<22}")
        sys.exit(0)
    except SDKException as err:
        log.error(err)
        sys.exit(1)
