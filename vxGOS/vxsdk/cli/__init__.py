"""
vxsdk.cli   - CLI abstration handling
"""
__all__ = [
    'vxsdk_cli_entry',
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
def vxsdk_cli_entry() -> None:
    """ Internal SDK for the vxGOS project """
