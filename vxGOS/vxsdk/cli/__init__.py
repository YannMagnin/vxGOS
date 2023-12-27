"""
vxsdk.cli   - CLI abstration handling
"""
__all__ = [
    'cli_entry',
]
import click

from vxsdk.cli.board import vxsdk_cli_board_entry

#---
# Public
#---

@click.group(
    'vxsdk',
    commands = [
        vxsdk_cli_board_entry,
    ],
)
def cli_entry() -> None:
    """ Internal SDK for the vxGOS project """
