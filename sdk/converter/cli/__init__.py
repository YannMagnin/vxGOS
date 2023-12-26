"""
cli.conv - assset conversion abstraction
"""

from core.logger import log

from cli.doctor import doctor_conv_cli
from cli.assets import assets_conv_cli
from cli.addin import addin_conv_cli

__all__ = [
    '__VXSDK_PLUGIN_META__',
    'cli_validate',
    'cli_parse',
]

#---
# Public
#---

__VXSDK_PLUGIN_META__ = (
    ['conv'],
    'vxGOS assets converter',
    """vxsdk-conv
Project assets converter

USAGE:
    vxsdk vxgos conv(-<ACTION>) [OPTIONS] ...

DESCRIPTION:
    Convert vxGOS project assets (or binary) into various form. By default, if
    no action is specified, the "assets" conversion is selected.

ACTIONS:
    asset       convert asset into source file or binary file
    addin       convert binary into addin file for vxOS
    doctor      try to display assets and addin information (debug)

See `vxsdk vxgos conv-<action> --help` for more information on a specific
action
"""
)

def cli_validate(name):
    """ validate the module name """
    return name.find('conv') == 0

def cli_parse(argv):
    """ Build subcommand entry """
    if '--help' in argv or '-h' in argv:
        log.user(__VXSDK_PLUGIN_META__[2])
        return 0
    if argv[0].find('conv-') != 0:
        argv[0] = 'conv-asset'
    action = argv[0][5:]
    if action == 'doctor':
        return doctor_conv_cli(argv[1:])
    if action == 'assets':
        return assets_conv_cli(argv[1:])
    if action == 'addin':
        return addin_conv_cli(argv[1:])
    log.error(f"unable to find action '{action}'")
    return 84
