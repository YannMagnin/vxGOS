"""
vxsdk.core.converter._font.font     - font asset abstraction
"""
__all__ = [
        'ConvAssetFont',
        ]
from typing import Dict, List, Any
from pathlib import Path

from vxsdk.core.converter.asset import ConvAsset
from vxsdk.core.converter._font.exception import ConvAssetFontException

#---
# Public
#---

# Allow too many instance attributes to facilitate object manipulation
# pylint: disable=locally-disabled,R0902
class ConvAssetFont(ConvAsset):
    """ font asset class
    """
    def __init__(
        self,
        name: str,
        config: Dict[str, Any],
        prefix_asset: Path,
    ) -> None:
        super().__init__(name, config, prefix_asset)
        self.charset            = 'normal'
        self.grid_size_x        = 0
        self.grid_size_y        = 0
        self.grid_padding       = 1
        self.grid_border        = 0
        self.is_proportional    = False
        self.line_height        = 0
        self.char_spacing       = 1
        self.glyph_size         = 0
        self.glyph_height       = 0
        self.font_size          = 0
        self.data: List[Any]    = []
        if 'charset' in config:
            if config['charset'] not in ['default', 'unicode']:
                raise ConvAssetFontException(
                    f"[{name}] Unknown charset '{config['charset']}'"
                )
            self.charset = config['charset']
        if 'grid_size' not in config:
            raise ConvAssetFontException(
                f"[{name}] Missing critical grid size information, abord"
            )
        try:
            grid_size = config['grid_size'].split('x')
            self.grid_size_x = int(grid_size[0])
            self.grid_size_y = int(grid_size[1])
            if 'grid_padding' in config:
                self.grid_padding = int(config['grid_padding'])
            if 'grid_border' in config:
                self.grid_border = int(config['grid_border'])
            if 'proportional' in config:
                self.is_proportional = config['proportional']
            self.line_height = self.grid_size_y
            if 'line_height' in config:
                self.line_height = config['line_height']
            if 'char_spacing' in config:
                self.char_spacing = config['char_spacing']
            self.glyph_height = self.grid_size_y
        except ValueError as err:
            raise ConvAssetFontException(
                f"[{name}] unable to convert asset config ({err})"
            ) from err

    def __str__(self) -> str:
        content  = f"ConvAssetFont '{self.name}':\n"
        content += f"|-- charset: '{self.charset}'\n"
        content += f"|-- grid size X: {self.grid_size_x}\n"
        content += f"|-- grid size Y: {self.grid_size_y}\n"
        content += f"|-- grib border: {self.grid_border}\n"
        content += f"|-- proportional:{self.is_proportional}\n"
        content += f"|-- line height: {self.line_height}\n"
        content += f"`-- char spacing: {self.char_spacing}\n"
        return content

    #---
    # Public methods
    #---

    def generate(self, prefix_build: Path) -> Path:
        """ generate C file
        """
        raise ConvAssetFontException('generation Not implemented')
