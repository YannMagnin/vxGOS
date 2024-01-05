"""
vxsdk.core._config  - internal SDK configuration
"""
__all__ = [
    'CONFIG_SDK_PREFIX_SRCS',
    'CONFIG_SDK_PREFIX_BOARDS',
    'CONFIG_SDK_PREFIX_BUILD',
]
from pathlib import Path

#---
# Public
#---

CONFIG_SDK_PREFIX_SRCS   = Path(f"{__file__}/../../../vxgos").resolve()
CONFIG_SDK_PREFIX_BOARDS = CONFIG_SDK_PREFIX_SRCS/'boards'
CONFIG_SDK_PREFIX_BUILD  = (CONFIG_SDK_PREFIX_SRCS/'../../.vxsdk').resolve()
