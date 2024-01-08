"""
vxsdk.core.converter.manager    - converter manager
"""
__all__ = [
    'converter_manager_iterate',
]
from typing import Generator
from pathlib import Path

import toml

from vxsdk.core.logger import log
from vxsdk.core.converter.asset import ConvAsset
from vxsdk.core.converter._image import ConvAssetImage
from vxsdk.core.converter._font import ConvAssetFont
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
                    if 'type' not in item[1]:
                        raise ConverterException(
                            f"[{item[0]}] missing type information"
                        )
                    if item[1]['type'] == 'font':
                        yield ConvAssetFont(item[0], item[1], prefix_asset)
                        continue
                    if item[1]['type'] == 'image':
                        yield ConvAssetImage(item[0], item[1], prefix_asset)
                        continue
                    raise ConverterException(
                        f"[{item[0]}] asset type '{item[1]['type']}' "
                        'is not known'
                    )
                except ConverterException as err:
                    log.error(err)
                    continue
        except toml.TomlDecodeError as err:
            log.error(err)
