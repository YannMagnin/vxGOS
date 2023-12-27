"""
pylint - checker for vxnorm

This file does not expose an explicite VxChecker object declaration to avoid
dependencies handling, you just need to provide:
    ======================= ===============================================
    parse_file()            Parse the source file
    ======================= ===============================================
"""
import subprocess

#---
# Public
#---

def parse_file(checker, _, pathinfo):
    """ parse the mapped file

    The file is mapped using mmap() and seeked through offset 0 to avoid too
    many I/O operations with classical file primitive.

    @args
    > checker (VxChecker) - current checker instance for this file
    > mfile        (mmap) - mmap instance of the file, seeked at 0
    > pathname      (str) - file pathname

    @return
    > Nothing
    """
    status = subprocess.run(
        ['pylint', pathinfo['filepath']], capture_output=True, check=False
    )
    if status.returncode == 0:
        return

    for line in status.stdout.decode('utf8').split('\n'):
        if not line:
            continue
        if line[0] == '*':
            continue
        if line[0] == '-':
            break
        if not (line := line.split(' ', 2)):
            continue
        checker.notify(
            line[0].split(':', 1)[1][:-1],
            f"[{line[1][:-1]}] {line[2]}",
            'pylint'
        )
