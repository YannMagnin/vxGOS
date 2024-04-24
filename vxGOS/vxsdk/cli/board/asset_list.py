"""
vxsdk.cli.board.asset-list  - asset listing for the selected board
"""
__all__ = [
    'vxsdk_cli_board_asset_list_entry',
]
from typing import NoReturn
import sys

import click

from vxsdk.core.exception import SDKException
from vxsdk.core.converter import converter_manager_iterate
from vxsdk.core.board.manager import board_manager_select_get
from vxsdk.core._config import CONFIG_SDK_PREFIX_SRCS
from vxsdk.core.logger import log

#---
# Public
#---

@click.command('asset-list')
@click.option(
    '-p', '--project', 'project',
    required    = True,
    metavar     = 'PROJECT_NAME',
    help        = 'project to list assets',
    type        = click.Choice(
        choices         = ['bootloader', 'kernel', 'os'],
        case_sensitive  = False,
    ),
)
def vxsdk_cli_board_asset_list_entry(project: str) -> NoReturn:
    """ list all assets found in prefix
    """
    try:
        if not board_manager_select_get():
            raise SDKException('No board selected, abord')
        for asset in converter_manager_iterate(
            CONFIG_SDK_PREFIX_SRCS/f"{project}/assets/",
        ):
            log.user(asset)
        sys.exit(0)
    except SDKException as err:
        log.error(err)
        sys.exit(1)
