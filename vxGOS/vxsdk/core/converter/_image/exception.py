"""
vxsdk.core.converter._image.exception   - image asset exception abstraction
"""
__all__ = [
    'ConvAssetImageException',
]

from vxsdk.core.converter.exception import ConverterException

#---
# Public
#---

class ConvAssetImageException(ConverterException):
    """ general asset image exception """
