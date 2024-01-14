"""
vxsdk.cli.board.build   - build abstraction
"""
__all__ = [
    'vxsdk_cli_board_build_entry',
]
from typing import NoReturn, Optional
import sys

import click

from vxsdk.core.exception import SDKException
from vxsdk.core.board import board_manager_build

#---
# Public
#---

@click.command('build')
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
    '-v', '--verbose', 'enable_verbose',
    is_flag     = True,
    required    = False,
    help        = 'enbable verbose for CMake operation',
)
def vxsdk_cli_board_build_entry(
    project_target: Optional[str],
    enable_verbose: bool,
) -> NoReturn:
    """ build the selected board
    """
    try:
        img_path = board_manager_build(project_target, enable_verbose)
        print(f"Final generated at '{str(img_path)}'")
        sys.exit(0)
    except SDKException as err:
        print(err, file=sys.stderr)
        sys.exit(0)
