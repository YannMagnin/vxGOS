"""
vxsdk.cli.board     - board management
"""
__all__ = [
    'vxsdk_cli_board_entry',
]
import click

from vxsdk.cli.board.list import vxsdk_cli_board_list_entry
from vxsdk.cli.board.select import vxsdk_cli_board_select_entry
from vxsdk.cli.board.build import vxsdk_cli_board_build_entry
from vxsdk.cli.board.asset_list import vxsdk_cli_board_asset_list_entry
from vxsdk.cli.board.asset_build import vxsdk_cli_board_asset_build_entry

#---
# Public
#---

@click.group(
    'board',
    commands = [
        vxsdk_cli_board_list_entry,
        vxsdk_cli_board_select_entry,
        vxsdk_cli_board_build_entry,
        vxsdk_cli_board_asset_list_entry,
        vxsdk_cli_board_asset_build_entry,
    ],
)
def vxsdk_cli_board_entry() -> None:
    """ vxGOS board management """
