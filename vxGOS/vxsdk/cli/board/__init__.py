"""
vxsdk.cli.board     - board management
"""
__all__ = [
    'vxsdk_cli_board_entry',
]
import click

from vxsdk.cli.board.list import vxsdk_cli_board_list_entry

#---
# Public
#---

@click.group(
    'board',
    commands = [
        vxsdk_cli_board_list_entry,
    ],
)
def vxsdk_cli_board_entry() -> None:
    """ vxGOS board management """
