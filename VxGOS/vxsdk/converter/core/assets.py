"""
core.conv.assets - Vhex assets converter
"""
import os
import toml

from core.logger import log
from core.font import font_generate
from core.image import image_generate

__all__ = [
    'assets_generate'
]

#---
# Internals
#---

class _VxAssetException(Exception):
    """ simple exception wrapper """

class _VxAsset():
    """Represent a asset object

    This is an internal class which represents assets information with some
    methods to abstract conversion and file type manipulation (for asset type
    font and asset type bitmap).

    Also note that this class is private because we use a tricky optimization
    to parse the `vxconv.txt` file, this is why we have no "private" property
    with setter and getter, and why this class is "hidden".

    Some important methods to note:

    ================================== =======================================
    Name                               Description
    ================================== =======================================
      generate()                       Generate the source file (C)
    ================================== =======================================

    """
    def __init__(self, prefix, name, meta):
        if 'path' not in meta:
            raise _VxAssetException(f"[{name}] missing path information")
        if 'type' not in meta:
            raise _VxAssetException(f"[{name}] missing type information")
        if meta['type'] not in ['font', 'image']:
            raise _VxAssetException(f"asset type '{meta[type]}' is not known")

        self._name = name
        self._meta = meta
        self._type = meta['type']
        self._path = prefix + '/' + meta['path']
        if not os.path.exists(self.path):
            raise _VxAssetException(
                "asset path '{self._path}' cannot be openned"
            )

    def __repr__(self):
        return f'<_VxAssetObj, {self.name}>'

    def __str__(self):
        content  = f"[{self.name}]\n"
        content += f" - type: {self.type}\n"
        content += f" - path: {self.path}\n"
        return content

    #---
    # Getter
    #---

    @property
    def path(self):
        """<property> path"""
        return self._path

    @property
    def name(self):
        """<property> name"""
        return self._name

    @property
    def type(self):
        """<property> type"""
        return self._type

    @property
    def meta(self):
        """<property> meta"""
        return self._meta

    #---
    # Public method
    #---

    def generate_source_file(
        self,
        prefix_output,
        generator,
        endianness,
        force_generate
    ):
        """generate source file """
        function = font_generate if self.type == 'font' else image_generate
        return function(
            self,
            prefix_output,
            generator,
            endianness,
            force_generate
        )

#---
# Public
#---

def assets_generate(info):
    """ Walk through the assets prefix and generate all source file

    @args
    > info  (dataclass) - contains all information about the conversion

    @return
    > a list of all generated sources pathname
    """
    if not os.path.exists(info.prefix_output):
        os.makedirs(info.prefix_output)
    for root, _, files in os.walk(info.prefix_assets):
        if 'vxconv.toml' not in files:
            continue
        with open(f"{root}/vxconv.toml", 'r', encoding='utf-8') as inf:
            content = toml.loads(inf.read())
            for asset_name in content:
                if info.assets_filter:
                    if asset_name not in info.assets_filter:
                        continue
                log.debug(f"converting {asset_name}...")
                log.user(
                    _VxAsset(
                        root,
                        asset_name,
                        content[asset_name]
                    ).generate_source_file(
                        info.prefix_output,
                        info.generator,
                        info.endianness,
                        info.force
                    )
                )
    return 0
