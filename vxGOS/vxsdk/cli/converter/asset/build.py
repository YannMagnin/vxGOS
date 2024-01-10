"""
vxsdk.cli.converter.build   - build asset information
"""
__all__ = [
    'vxsdk_cli_converter_asset_build_entry',
]
from typing import NoReturn
from pathlib import Path
import sys

import click

from vxsdk.core.exception import SDKException
from vxsdk.core.converter.manager import converter_manager_generate

#---
# Public
#---

@click.command('build')
@click.option(
    '-a', '--asset', 'prefix_asset',
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
@click.option(
    '-b', '--build', 'prefix_build',
    required    = True,
    metavar     = 'PREFIX_BUILD',
    help        = 'prefix to build assets',
    type        = click.Path(
        exists          = False,
        file_okay       = False,
        dir_okay        = True,
        path_type       = Path,
        resolve_path    = True,
    )
)
@click.option(
    '-p', '--project', 'project_target',
    required    = False,
    type        = click.Choice(
        [
            'bootloader',
            'kernel',
            'os',
        ],
        case_sensitive  = False,
    )
)
@click.option(
    '-e', '--endianness', 'endianness',
    required    = True,
    type        = click.Choice(
        [
            'little',
            'big',
        ],
        case_sensitive  = False,
    )
)
def vxsdk_cli_converter_asset_build_entry(
    prefix_asset:   Path,
    prefix_build:   Path,
    project_target: str,
    endianness:     str,
) -> NoReturn:
    """ build asset
    """
    try:
        converter_manager_generate(
            prefix_asset,
            prefix_build,
            project_target,
            endianness,
        )
        sys.exit(0)
    except SDKException as err:
        print(err, file=sys.stderr)
        sys.exit(1)
