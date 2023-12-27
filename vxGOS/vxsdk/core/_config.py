"""
vxsdk.core._config  - internal SDK configuration
"""
__all__ = [
    'CONFIG_SDK_PREFIX_BOARDS',
]
from pathlib import Path

#---
# Public
#---

CONFIG_SDK_PREFIX_BOARDS = Path(f"{__file__}/../../../vxgos/boards").resolve()
