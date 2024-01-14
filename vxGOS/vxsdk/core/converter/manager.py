"""
vxsdk.core.converter.manager    - converter manager
"""
__all__ = [
    'converter_manager_iterate',
    'converter_manager_generate',
]
from typing import Generator, Dict, Any, List
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

def converter_manager_generate(
    prefix_asset:   Path,
    prefix_include: Path,
    prefix_build:   Path,
    project_target: str,
    compconf:       Dict[str,Any],
) -> Path:
    """ generate all asset C files
    """
    need_build = 0
    asset_outfile_list: List[Path] = []
    for asset in converter_manager_iterate(prefix_asset):
        asset_info = asset.generate(
            prefix_build,
            project_target,
            compconf['toolchain']['endianness'],
        )
        asset_outfile_list.append(asset_info[0])
        need_build += asset_info[1]
    if need_build == 0:
        return prefix_build/'_build/libassets.a'
    return converter_cmake_build(
        prefix_build,
        prefix_include,
        compconf,
        asset_outfile_list,
    )
