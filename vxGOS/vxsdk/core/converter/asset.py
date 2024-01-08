"""
vxsdk.core.converter.asset  - asset abstraction
"""
__all__ = [
    'ConvAsset',
]
from typing import Dict, Any, cast
from pathlib import Path
from abc import ABC, abstractmethod

from vxsdk.core.converter.exception import ConverterException

#---
# Public
#---

class ConvAsset(ABC):
    """ common asset abstraction
    """
    def __init__(
        self,
        name: str,
        config: Dict[str, Any],
        prefix_asset: Path,
    ) -> None:
        if 'path' not in config:
            raise ConverterException(f"[{name}] missing path information")
        if 'type' not in config:
            raise ConverterException(f"[{name}] missing type information")
        if config['type'] not in ['font', 'image']:
            raise ConverterException(
                f"[{name}] asset type '{config['type']}' is not known"
            )
        self._name = name
        self._config = config
        self._image_path = prefix_asset/cast(str, config['path'])
        if not self._image_path.exists():
            raise ConverterException(
                f"[{name}] assiciated path '{self._image_path}' does "
                'not exists'
            )

    def __repr__(self) -> str:
        return f"<ConvAsset, {self.name}>"

    #---
    # Public property
    #---

    @property
    def name(self) -> str:
        """ return the asset name """
        return self._name

    @property
    def type(self) -> str:
        """ return the asset type """
        return cast(str, self._config['type'])

    @property
    def image_path(self) -> Path:
        """ return the associated image path """
        return self._image_path

    #---
    # Public methods
    #---

    @abstractmethod
    def generate(self, prefix_build: Path) -> Path:
        """
        generate the C file assiciated to the asset and return the path
        of the said generated C file
        """
