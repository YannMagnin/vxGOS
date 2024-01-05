"""
vxsdk.core.board._kernel    - kernel abstraction
"""
__all__ = [
    'board_kernel_initialise',
]
from vxsdk.core.board._common import board_common_initialise
from vxsdk.core._config import (
    CONFIG_SDK_PREFIX_BUILD,
    CONFIG_SDK_PREFIX_SRCS,
)

#---
# Public
#---

def board_kernel_initialise(board_name: str) -> None:
    """ initialise the build information needed for the kernel
    """
    board_common_initialise(
        'kernel',
        board_name,
        CONFIG_SDK_PREFIX_SRCS/'kernel',
        CONFIG_SDK_PREFIX_BUILD/f"{board_name}/kernel",
        CONFIG_SDK_PREFIX_SRCS/f"kernel/boards/{board_name}/{board_name}.ld",
    )
