"""
vxsdk.core.converter._image.image   - image asset abstraction
"""
__all__ = [
    'ConvAssetImage',
]
from pathlib import Path

from vxsdk.core.converter.asset import ConvAsset
from vxsdk.core.converter._image.exception import ConvAssetImageException

#---
# Public
#---

class ConvAssetImage(ConvAsset):
    """ image asset class
    """

    #---
    # Public methods
    #---

    def generate(self, prefix_build: Path) -> Path:
        """ generate C file
        """
        raise ConvAssetImageException('generation Not implemented')
