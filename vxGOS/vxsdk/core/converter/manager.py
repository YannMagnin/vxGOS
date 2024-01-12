"""
vxsdk.core.converter.manager    - converter manager
"""
__all__ = [
    'converter_manager_iterate',
    'converter_manager_generate',
    'converter_manager_load_compconf',
]
from typing import Optional, Generator, Dict, Any, List
from pathlib import Path

import toml

from vxsdk.core.logger import log
from vxsdk.core.converter.asset import ConvAsset
from vxsdk.core.converter.exception import ConverterException
from vxsdk.core.converter._cmake import converter_cmake_build

#---
# Public
#---

def converter_manager_iterate(
    prefix_asset: Path,
) -> Generator[ConvAsset,None,None]:
    """ iterate over all assets
    """
    for convfile in prefix_asset.rglob('vxconv.toml'):
        try:
            for item in toml.load(convfile).items():
                try:
                    yield ConvAsset.factory_load(
                        item[0],
                        item[1],
                        prefix_asset,
                    )
                except ConverterException as err:
                    log.error(err)
                    continue
        except toml.TomlDecodeError as err:
            log.error(err)

def converter_manager_load_compconf(compconf_file: Path) -> Dict[str,Any]:
    """ load the compiles.toml file
    """
    try:
        compile_conf = toml.load(compconf_file)
        if 'toolchain' not in compile_conf:
            log.emergency('Missing toolchain information')
        if 'prefix' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain prefix information')
        if 'processor' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain processor information')
        if 'cflags' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain cflags information')
        if 'ldflags' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain ldflags information')
        if 'libraries' not in compile_conf['toolchain']:
            log.emergency('Missing toolchain endianness information')
        return compile_conf
    except toml.TomlDecodeError as err:
        log.emergency(err)

def converter_manager_generate(
    prefix_asset:   Path,
    prefix_include: Path,
    prefix_build:   Path,
    project_target: str,
    compconf:       Dict[str,Any],
) -> Optional[Path]:
    """ generate all asset C files
    """
    if 'converter' not in compconf:
        raise ConverterException(
            'Missing converter entry in compiles.toml for board'
        )
    if 'endianness' not in compconf['converter']:
        raise ConverterException(
            'Missing converter endianness information in compiles.toml'
        )
    asset_outfile_list: List[Path] = []
    for asset in converter_manager_iterate(prefix_asset):
        asset_outfile_list.append(
            asset.generate(
                prefix_build,
                project_target,
                compconf['converter']['endianness'],
            ),
        )
    return converter_cmake_build(
        prefix_build,
        prefix_include,
        compconf,
        asset_outfile_list,
    )
