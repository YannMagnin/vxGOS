"""
vxsdk.cli.converter.list    - list assets information
"""
__all__ = [
    'vxsdk_cli_converter_asset_list_entry',
]
from typing import NoReturn
from pathlib import Path
import sys

import click

from vxsdk.core.exception import SDKException
from vxsdk.core.converter import converter_manager_iterate
from vxsdk.core.logger import log

#---
# Public
#---

@click.command('list')
@click.option(
    '-p', '--prefix', 'prefix_asset',
    required    = True,
    metavar     = 'PREFIX_ASSET',
    help        = 'prefix to search assets',
    type        = click.Path(
        exists          = True,
        file_okay       = False,
        dir_okay        = True,
        path_type       = Path,
        resolve_path    = True,
    )
)
def vxsdk_cli_converter_asset_list_entry(prefix_asset: Path) -> NoReturn:
    """ list all assets found in prefix
    """
    try:
        for asset in converter_manager_iterate(prefix_asset):
            log.user(asset)
        sys.exit(0)
    except SDKException as err:
        log.error(err)
        sys.exit(1)
