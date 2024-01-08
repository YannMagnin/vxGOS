"""
vxsdk.cli.converter.asset   - SDK asset converter
"""
__all__ = [
    'vxsdk_cli_converter_asset_entry',
]
import click

from vxsdk.cli.converter.asset.list import (
    vxsdk_cli_converter_asset_list_entry,
)

#---
# Public
#---

@click.group(
    'assets',
    commands = [
        vxsdk_cli_converter_asset_list_entry,
    ]
)
def vxsdk_cli_converter_asset_entry() -> None:
    """ converter asset entry """
