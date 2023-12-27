"""
core.unitests.gcovr - gcovr abstraction
"""
import os
import subprocess

from core.logger import log

#---
# Internals
#---

def _gcovr_parse_output(output, covered_list):
    """ fetch only line information
    """
    lineinfo = []
    workaround_line = None
    for line in output.decode('ascii').split('\n'):
        if not (line := line.split()):
            continue
        if workaround_line:
            lineinfo.append(workaround_line + line)
            workaround_line = None
            continue
        find = False
        for filename in covered_list:
            if filename.find(line[0]) >= 0:
                find = True
                break
        if not find:
            continue
        if len(line) < 2:
            workaround_line = line
            continue
        lineinfo.append(line)
    return lineinfo

def _gcovr_display_output(lineinfo):
    """ proper display line information
    """
    maxwidth = 0
    for line in lineinfo:
        maxwidth = max(len(line[0]), maxwidth)
    for line in lineinfo:
        percent = int(line[3][:-1])
        color = '\033[31m'
        if 50 <= percent <= 90:
            color = '\033[33m'
        if percent >= 90:
            color = '\033[32m'
        if percent == 100:
            log.user(f"[\033[92m{line[3]: >4}\033[0m] {line[0]: <{maxwidth}}")
            continue
        log.user(
            f"[{color}{line[3]: >4}\033[0m] {line[0]: <{maxwidth}} {line[4]}"
        )
#---
# Public
#---

def gcovr_unitests_run(bininfo):
    """ run code coverage
    """
    pwd  = os.path.dirname(bininfo[1])
    cmd  = f"gcovr {pwd}"
    proc = subprocess.run(cmd.split(), capture_output=True, check=False)
    if proc.returncode != 0:
        log.error(f"{bininfo[0]}: code coverage error, skipped")
        log.error(proc.stderr.decode('utf8'))
        return -1
    lineinfo = _gcovr_parse_output(proc.stdout, bininfo[2]['covered'])
    _gcovr_display_output(lineinfo)
    return 0
