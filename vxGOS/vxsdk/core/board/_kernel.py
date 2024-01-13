"""
vxsdk.core.board._kernel    - kernel abstraction
"""
__all__ = [
    'board_kernel_initialise',
]

#---
# Public
#---

def board_kernel_initialise(_: str) -> None:
    """ initialise the build information needed for the kernel
    """
