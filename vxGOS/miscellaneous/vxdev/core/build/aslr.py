"""
core.aslr - generate ASLR symbols table
"""
import subprocess

from core.logger import log

__all__ = [
    'aslr_generate'
]

#---
# Internals
#---

def _fetch_symtab(binpath):
    """
    """
    ret = subprocess.run(
        f"readelf -r {binpath}".split(),
        capture_output=True,
        check=False
    )
    if ret.returncode != 0:
        log.error(ret.stderr.decode('utf8'))
    symtab = []
    for i, line in enumerate(ret.stdout.decode('utf8').splitlines()):
        if i < 3:
            continue
        if len(sinfo := line.split()) != 7:
            continue
        log.debug(f"[{i}] {sinfo}")
        symtab.append(sinfo)
    return symtab

def _fetch_sectab(binpath):
    """
    """
    ret = subprocess.run(
        f"readelf --sections --wide {binpath}".split(),
        capture_output=True,
        check=False
    )
    if ret.returncode != 0:
        log.error(ret.stderr.decode('utf8'))
    sectab = []
    for i, line in enumerate(ret.stdout.decode('utf8').splitlines()):
        if line.find('  [') != 0:
            continue
        secinfo = line[7:].split()
        log.debug(f"[{i}] {secinfo[0]}")
        sectab.append(secinfo)
    return sectab

#---
# Public
#---

def aslr_generate(build, binpath):
    """ generate ASLR symbols table
    """
    symtab = _fetch_symtab(binpath)
    sectab = _fetch_sectab(binpath)
    return build.postscript['alsr'](binpath, symtab, sectab)
