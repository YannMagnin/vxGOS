"""
core.unitests.requirements - requirements.txt file parser
"""
import os

from core.logger import log

__all__ = [
    'requirements_unitests_parse'
]

#---
# Public
#---

def requirements_unitests_parse(prefix, suite, test):
    """ parse requirements.txt file for unitests

    @args
    > prefix   (str) - unitests root dir
    > suite    (str) - unitests suite (bootloader, kernel, ...)
    > test     (str) - unitests name

    @return
    > a dictionary with parsed information
    """
    testname = f"{suite}:{test}"
    pathname = f"{prefix}/unitests/{test}/requirements.txt"
    if not os.path.exists(prefix):
        log.emergency(f"{pathname}: file doesn't exists, skipped")

    line_id = 0
    requirement = {
        'covered'      : [],
        'dependencies' : [],
        'includes'     : []
    }
    mapping = {
        'c' : requirement['covered'],
        'd' : requirement['dependencies'],
        'i' : requirement['includes'],
    }
    with open(pathname, 'r', encoding='ascii') as reqfile:
        while True:
            line_id += 1
            if not (line := reqfile.readline()):
                break
            if not (line := line.strip()):
                continue
            if line.strip()[0] == '#':
                continue
            line = line.split()
            if line[0][0] != '[' or line[0][2] != ']':
                log.warn(f"{testname}:requirements.txt:{line_id}: line error")
                continue
            if line[0][1] not in mapping:
                log.warn(f"{testname}: unsupported '{line[0][1]}' flag")
                continue
            real_filename = f"{prefix}/{line[1]}"
            if not os.path.exists(real_filename):
                log.emergency(
                    f"{testname}: dependency '{real_filename}'"
                     "not exists"
                )
            mapping[line[0][1]].append(real_filename)

    log.debug(f"- requirements.txt : {requirement}")
    return requirement
