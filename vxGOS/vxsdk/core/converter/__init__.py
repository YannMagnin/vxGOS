"""
vxsdk.core.converter    - asset converter module
"""
__all__ = [
    'ConvAsset',
    'ConverterException',
    'converter_manager_iterate',
]
from vxsdk.core.converter.asset import ConvAsset
from vxsdk.core.converter.exception import ConverterException
from vxsdk.core.converter.manager import (
    converter_manager_iterate,
)

# force import ConAsset subclass to trigger the mechanism that register
# all asset type and factory magic
# (todo) : move to auto loading classes as internal pluging mechanism
from vxsdk.core.converter._font import ConvAssetFont
from vxsdk.core.converter._image import ConvAssetImage
