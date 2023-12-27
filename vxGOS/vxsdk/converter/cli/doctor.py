"""
cli.conv.doctor - vxSDK converter doctor.
"""
import sys

from core.logger import log

__all__ = [
    'doctor_conv_cli'
]

#---
# Public
#---

def doctor_conv_cli(_):
    """Process CLI handling

    TODO:
    > give asset file description to check error
    > try to display asset and addin information based on the project type
    """
    log.warn('conv: doctor action not implemented yet')
    sys.exit(84)
