"""
vxsdk.core.board._kernel    - kernel abstraction
"""
__all__ = [
    'board_kernel_initialise',
    'board_kernel_build',
]
from typing import Any
from pathlib import Path

import toml

from vxsdk.core.logger import log
from vxsdk.core.board.exception import BoardException
from vxsdk.core.converter.manager import converter_manager_generate
from vxsdk.core._config import (
    CONFIG_SDK_PREFIX_BUILD,
    CONFIG_SDK_PREFIX_SRCS,
)

#---
# Internals
#---

def _board_kernel_load_conf(board_name: str) -> dict[str,Any]:
    """ try to load the kernel config.toml file
    """
    kernel_conf_path = Path(
        CONFIG_SDK_PREFIX_SRCS/f"kernel/boards/{board_name}/config.toml"
    )
    if not kernel_conf_path.exists():
        raise BoardException(
            'Unable to find the kernel configuration file at '
            f"'{str(kernel_conf_path)}'"
        )
    try:
        kernel_config = toml.load(kernel_conf_path)
    except toml.TomlDecodeError as err:
        raise BoardException(
            'Unable to load the kernel configuration file at '
            f"'{str(kernel_conf_path)}': TomlDecodeError: {err}"
        ) from err
    error = None
    if 'kernel' not in kernel_config:
        error = 'Missing kernel entry in kernel configuration file'
    if 'modules' not in kernel_config['kernel']:
        error = 'Missing kernel modules information in configuration file'
    if 'drivers' not in kernel_config['kernel']:
        error = 'Missing kernel drivers information in configuration file'
    if error is not None:
        raise BoardException(error)
    prefix_srcs = CONFIG_SDK_PREFIX_SRCS/'kernel/src'
    for part in ('modules', 'drivers'):
        for target in kernel_config['kernel'][part]:
            if (prefix_srcs/f"{part}/{target}").exists():
                continue
            raise BoardException(
                f"Unable to find the {part} named {target}"
            )
    return kernel_config

def _board_kernel_find_srcs(
    prefix: Path,
    board_name: str,
    board_config: dict[str,Any],
) -> dict[str,Any]:
    """ find all files needed to generate the cmakefile
    """
    srcs_list: list[Path] = []
    srcs_list += list((prefix/'src').glob('*.[csS]'))
    srcs_list += list((prefix/f"boards/{board_name}/src").rglob('*.[csS]'))
    for part in ('modules', 'drivers'):
        for target in board_config['kernel'][part]:
            srcs_list += list(
                (prefix/f"src/{part}/{target}").rglob('*.[csS]')
            )
    return {
        'include' : (
            prefix/f"boards/{board_name}/include",
            prefix/'include',
        ),
        'srcs' : srcs_list,
    }

#---
# Public
#---

def board_kernel_initialise(
    board_name: str,
    board_config: dict[str,Any],
) -> None:
    """ initialise the build information needed for the kernel
    """
    log.user('[+] loading kernel configuration')
    if 'kernel' in board_config:
        log.warning(
            'provided kernel entries in generic board configuration '
            'will be ignored'
        )
    board_config.update(_board_kernel_load_conf(board_name))
    prefix_build = CONFIG_SDK_PREFIX_BUILD/f"{board_name}/kernel"
    prefix_build.mkdir(parents=True, exist_ok=True)

def board_kernel_build(
    board_name: str,
    board_config: dict[str,Any],
    generator:      dict[str,Any],
) -> Path:
    """ generate the ELF kernel information
    """
    log.user('[+] preliminary checks...')
    board_kernel_initialise(board_name, board_config)
    prefix_build = CONFIG_SDK_PREFIX_BUILD/f"{board_name}/kernel"
    prefix_src   = CONFIG_SDK_PREFIX_SRCS/'kernel'
    compile_file = _board_kernel_find_srcs(
        prefix_src,
        board_name,
        board_config,
    )
    print(compile_file)
    log.user('[+] building assets...')
    asset_library = converter_manager_generate(
        prefix_src/'assets/',
        prefix_src/'include/',
        prefix_build/'_assets/',
        'kernel',
        board_config,
    )
    board_config['toolchain']['libraries'].insert(0, '-lassets')
    board_config['toolchain']['ldflags'].append(
        f"-L{str(asset_library.parent)}",
    )
    exit(84)
    return Path('/tmp')
    # (todo) : building the kernel
    # (todo) : construct the kernel image
