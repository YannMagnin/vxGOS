"""
vxsdk.cli.board.asset-build     - asset build for the selected board
"""
__all__ = [
    'vxsdk_cli_board_asset_build_entry',
]
from typing import NoReturn
import sys
import os

import click

from vxsdk.core.exception import SDKException
from vxsdk.core.converter.manager import converter_manager_generate
from vxsdk.core.board.manager import board_manager_select_get
from vxsdk.core._config import CONFIG_SDK_PREFIX_SRCS
from vxsdk.core.board._config import board_config_load
from vxsdk.core.logger import log

#---
# Public
#---

@click.command('asset-build')
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
@click.option(
    '-v', '--verbose', 'enable_verbose',
    is_flag     = True,
    required    = False,
    help        = 'enbable verbose for CMake operation',
)
def vxsdk_cli_board_asset_build_entry(
    project: str,
    enable_verbose: bool,
) -> NoReturn:
    """ build assets for a specific project
    """
    try:
        if not (board := board_manager_select_get()):
            raise SDKException('No board selected, abord')
        if enable_verbose:
            os.environ['VERBOSE'] = "1"
        config = board_config_load(board.prefix_build/'compiles.toml')
        if not (
            asset_lib_path := converter_manager_generate(
                CONFIG_SDK_PREFIX_SRCS/f"{project}/assets/",
                CONFIG_SDK_PREFIX_SRCS/f"{project}/include/",
                board.prefix_build/f"{project}/_assets/",
                project,
                config,
            )
        ):
            log.error('No assets found')
            sys.exit(0)
        log.user(f"Asset library generate at {str(asset_lib_path)}")
        sys.exit(0)
    except SDKException as err:
        log.error(err)
        sys.exit(1)
