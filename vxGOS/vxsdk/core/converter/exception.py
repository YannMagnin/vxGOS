"""
vxsdk.core.converter.exception  - general converter exception
"""
__all__ = [
    'ConverterException',
]

from vxsdk.core.exception import SDKException

#---
# Public
#---

class ConverterException(SDKException):
    """ general converter exception class """
