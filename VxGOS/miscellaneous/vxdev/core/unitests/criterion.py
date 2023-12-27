"""
unitests.criterion - criterion abstraction
"""
import os
import glob
import subprocess

from core.logger import log

__all__ = [
    'criterion_unitests_run'
]

#---
# Public
#---

def criterion_unitests_run(bininfo):
    """ run criterion unitests (purge converage informaiton)
    """
    log.debug(f"{bininfo[0]}: purge GCDA file...")
    query = f"{os.path.dirname(bininfo[1])}/**/*.gcd[ao]"
    for gcda in glob.glob(query, recursive=True):
        log.debug(f"- {gcda}...")
        os.remove(gcda)
    if subprocess.run([bininfo[1]], check=False).returncode != 0:
        log.error(f"{bininfo[0]}: unitest error, skipped code coverage")
        return -2
    return 0
