"""
vxsdk.cli.converter.addin   - vxGOS addin (application) converter
"""
__all__ = [
    'vxsdk_cli_converter_addin_entry',
]
from typing import Optional, NoReturn
from pathlib import Path
import sys

import click

from vxsdk.core.exception import SDKException

#---
# Public
#---

@click.command('addin')
@click.option(
    '-b', '--binary', 'binary_path',
    required    = True,
    metavar     = 'BIN_PATH',
    help        = 'raw binary file to convert',
    type        = click.Path(
        path_type       = Path,
        dir_okay        = False,
        file_okay       = True,
        exists          = True,
        resolve_path    = True,
    )
)
@click.option(
    '-i', '--icon', 'icon_path',
    required    = False,
    metavar     = 'ICON_PATH',
    help        = '92x62 icon file for the application',
    type        = click.Path(
        path_type       = Path,
        dir_okay        = False,
        file_okay       = True,
        exists          = True,
        resolve_path    = True,
    )
)
@click.option(
    '-o', '--output', 'output_path',
    required    = False,
    metavar     = 'OUTPUT_PATH',
    help        = 'generated addin output path',
    type        = click.Path(
        path_type       = Path,
        dir_okay        = True,
        file_okay       = False,
        exists          = True,
        resolve_path    = True,
    )
)
@click.option(
    '-v', '--version', 'version',
    required    = False,
    help        = 'addin version',
)
def vxsdk_cli_converter_addin_entry(
    binary_path:    Path,
    icon_path:      Optional[Path],
    output_path:    Optional[Path],
    version:        str,
) -> NoReturn:
    """ Convert binary file into Vhex OS application
    """
    try:
        print(
            'Convert binrary to addin:\n'
            f"> binary: {binary_path}\n"
            f"> icon: {icon_path}\n"
            f"> output: {output_path}\n"
            f"> version: {version}"
        )
        raise SDKException('Addin conversion not supported yet o(x_x)o')
    except SDKException as err:
        print(err, file=sys.stderr)
        sys.exit(0)
