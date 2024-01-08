"""
vxsdk.core.converter._font.exception    - font asset exception abstraction
"""
__all__ = [
    'ConvAssetFontException',
]

from vxsdk.core.converter.exception import ConverterException

#---
# Public
#---

class ConvAssetFontException(ConverterException):
    """ general asset image exception """
