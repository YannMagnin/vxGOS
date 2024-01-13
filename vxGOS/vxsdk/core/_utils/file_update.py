"""
vxsdk.core._utils.file_update   - file update helper
"""
__all__ = [
    'utils_file_update',
]
from pathlib import Path

#---
# Public
#---

def utils_file_update(file_pathname: Path, content: str) -> bool:
    """ check if the file need to be recreated
    """
    if file_pathname.exists():
        with open(file_pathname, 'r', encoding='ascii') as cmakefile:
            if cmakefile.read() == content:
                return False
        file_pathname.unlink()
    file_pathname.parent.mkdir(parents=True, exist_ok=True)
    with open(file_pathname, 'x', encoding='ascii') as toolfile:
        toolfile.write(content)
    return True
