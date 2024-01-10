"""
vxsdk.core.converter.manager    - converter manager
"""
__all__ = [
    'converter_manager_iterate',
    'converter_manager_generate',
]
from typing import Generator, List
from pathlib import Path

import toml

from vxsdk.core.logger import log
from vxsdk.core.converter.asset import ConvAsset
from vxsdk.core.converter.exception import ConverterException

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
    prefix_build:   Path,
    project_target: str,
    endianness:     str,
) -> List[Path]:
    """ generate all asset C files
    """
    asset_outfile_list: List[Path] = []
    for asset in converter_manager_iterate(prefix_asset):
        asset_outfile_list.append(
            asset.generate(
                prefix_build,
                project_target,
                endianness,
            ),
        )
    return asset_outfile_list
