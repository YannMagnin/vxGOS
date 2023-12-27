"""
vxsdk.cli.converter - asset converter CLI interface
"""
__all__ = [
    'vxsdk_cli_converter_entry',
]
import click

from vxsdk.cli.converter.addin import vxsdk_cli_converter_addin_entry

#---
# Public
#---

@click.group(
    'conv',
    commands = [
        vxsdk_cli_converter_addin_entry,
    ],
)
def vxsdk_cli_converter_entry() -> None:
    """ vxGOS asset converter """
