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
from vxsdk.core.board._config import board_config_load
from vxsdk.core.logger import log

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
    '-I', '--include', 'prefix_include',
    required    = True,
    metavar     = 'PREFIX_INCLUDE',
    help        = 'prefix to include directory',
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
        choices         = ['bootloader', 'kernel', 'os'],
        case_sensitive  = False,
    )
)
@click.option(
    '-c', '--compile', 'compconf_file',
    required    = True,
    metavar     = 'PREFIX_COMPILES',
    help        = 'compiles.toml file path',
    type        = click.Path(
        exists          = False,
        file_okay       = True,
        dir_okay        = False,
        path_type       = Path,
        resolve_path    = True,
    )
)
def vxsdk_cli_converter_asset_build_entry(
    prefix_asset:   Path,
    prefix_build:   Path,
    prefix_include: Path,
    project_target: str,
    compconf_file:  Path,
) -> NoReturn:
    """ build asset
    """
    try:
        converter_manager_generate(
            prefix_asset,
            prefix_build,
            prefix_include,
            project_target,
            board_config_load(compconf_file),
        )
        sys.exit(0)
    except SDKException as err:
        log.error(err)
        sys.exit(1)
