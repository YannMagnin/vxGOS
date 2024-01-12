"""
vxsdk.core.converter.asset  - asset abstraction
"""
__all__ = [
    'ConvAsset',
]
from typing import Dict, Any, Self, Tuple, cast
from pathlib import Path
from abc import ABC, abstractmethod
import re

from vxsdk.core.converter.exception import ConverterException

#---
# Public
#---

class ConvAsset(ABC):
    """ common asset abstraction
    """
    #---
    # Factory magic
    #---
    _subclass_list: Dict[str,Any] = {}

    def __init_subclass__(cls, /, **kwargs: Any) -> None:
        """ register all subclass and perform aggressif check on the name
        """
        super().__init_subclass__(**kwargs)
        if not (
            info := re.match(
                "(?P<cls>^ConvAsset(?P<key>[A-Z][a-z]+)$)",
                cls.__name__,
            )
        ):
            raise ConverterException(
                f"Subclass name '{cls.__name__}' is not valid, abord",
            )
        cls._subclass_list[info['key'].lower()] = cls

    @classmethod
    def factory_load(
        cls,
        asset_name: str,
        asset_conf: Dict[str,Any],
        asset_prefix: Path,
    ) -> Self:
        """ load appropriate class based on the type configuratio field
        """
        if 'type' not in asset_conf:
            raise ConverterException(
                f"[{asset_name}] missing type information"
            )
        if asset_conf['type'] in cls._subclass_list:
            return cast(
                Self,
                cls._subclass_list[asset_conf['type']](
                    asset_name,
                    asset_conf,
                    asset_prefix,
                ),
            )
        raise ConverterException(
            f"[{asset_name}] asset type '{asset_conf['type']}' "
            'is not known'
        )

    #---
    # Object magic
    #---

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
    def generate(
        self,
        prefix_build: Path,
        target:         str,
        endianness:     str,
    ) -> Tuple[Path,bool]:
        """
        generate the C file assiciated to the asset and return the path
        of the said generated C file
        """
        if endianness not in ['little', 'big']:
            raise ConverterException(
                "Not valid endianness, should be 'little' or 'big' "
                f"(get '{endianness}')"
            )
        if target not in ['bootloader', 'kernel', 'os']:
            raise ConverterException(
                "No valid target, should be 'bootloader', 'kernel' or 'os' "
                f"(get '{target}')"
            )
        return (prefix_build, False)
