"""
vxsdk.core.board.exception  - SDK board exception
"""
__all__ = [
    'BoardException',
]

from vxsdk.core.exception import SDKException

#---
# Public
#---

class BoardException(SDKException):
    """ Board exception class """
