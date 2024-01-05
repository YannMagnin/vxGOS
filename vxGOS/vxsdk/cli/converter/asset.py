"""
vxsdk.cli.converter.asset   - SDK asset converter
"""
__all__ = [
    'vxsdk_cli_converter_asset_entry',
]
from typing import NoReturn
from pathlib import Path
import sys

import click

from vxsdk.core.exception import SDKException

#---
# Public
#---

@click.command('assets')
@click.option(
    '-a', '--asset', 'asset_prefix',
    required    = True,
    metavar     = 'ASSET_PATH',
    help        = 'asset prefix to scan and convert',
    type        = click.Path(
        path_type       = Path,
        dir_okay        = True,
        file_okay       = False,
        exists          = True,
        resolve_path    = True,
    ),
)
@click.option(
    '-e', '--endianness', 'endianness',
    required    = False,
    help        = 'select endianness of converted assets',
    type        = click.Choice(
        [
            'big',
            'little',
        ],
        case_sensitive  = False,
    )
)
@click.option(
    '-t', '--target', 'target_format',
    required    = True,
    help        = 'conversion target',
    type        = click.Choice(
        [
            'bootloader',
            'kernel',
            'os',
        ],
        case_sensitive  = False,
    )
)
def vxsdk_cli_converter_asset_entry(
    asset_prefix:   Path,
    endianness:     str,
    target:         str,
) -> NoReturn:
    """ Convert all asset files in project directory

    Convert all assets file in the asset directory. This part of the
    converter module will scan the provided folder (or the current working
    directory) and will try to find all `vxconv.txt` file, which describe
    all assets that should be converted.

    If no argument is provided, then the current working directory is used
    as asset prefix and a storag for all generated source file. You can
    modify this behaviour using OPTIONS.

    For more information about the `vxconv.txt` in the wiki.
    """
    try:
        sys.exit(0)
    except SDKException as err:
        print(err, file=sys.stderr)
        sys.exit(0)
