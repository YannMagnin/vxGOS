"""
core.build.bootloader - bootloader build abstraction
"""
import subprocess
import glob

from core.logger import log
from core.build.cmake import cmake_build_configure
from core.build.compiles import compiles_fetch

__all__ = [
    'bootloader_configure',
]

#---
# Public
#---

def bootloader_configure(build, version):
    """ Configure the bootloader

    The configuration step is special. It will first fetch all common source
    files (stored at <vxgos/bootloader/src/>) then, fetch board-specific files
    like extra source file and the linker script.

    After theses operation, a CMake file will be generated in the build prefix
    (<env['VXSDK_BUILD_PREFIX']/bootloader/>)
    """
    prefix = build.prefix['bootloader']
    log.debug('fetching bootloader files...')
    log.debug(f"> prefix = {prefix}")

    log.debug('- fetching compiles informations...')
    compiles = compiles_fetch(f"{prefix}/boards/{build.target}/compiles.toml")
    if not compiles:
        log.emergency(
            "unable to find bootloader's compilation information "
            "(compiles.toml)"
        )

    assets_src = []
    if compiles.assets:
        log.debug('- generate assets sources files...')
        cmd  = '/usr/bin/env python3 '
        cmd += f"{prefix}/../../sdk/converter conv-assets "
        cmd += f"{prefix}/assets "
        cmd += f"{prefix}/../../.vxsdk/{build.target}/converter/bootloader "
        cmd += f"--filter={','.join(compiles.assets)} "
        cmd += '--bootloader '
        if compiles.toolchain_endian == 'little':
            cmd += '--little-endian'
        else:
            cmd += '--big-endian'
        proc = subprocess.run(cmd.split(), capture_output=True, check=False)
        if proc.returncode != 0:
            log.emergency(
                f"unable to convert bootloader assets : {proc.stderr}"
            )
        assets_src = proc.stdout.decode('utf8').split()
        log.debug(f"> assets src = {assets_src}")

    common_src = f"{prefix}/src/**/*.[csS]"
    common_src = glob.glob(common_src, recursive=True)
    log.debug(f"> common src = {common_src}")

    board_src = f"{prefix}/boards/{build.target}/src/**/*.[csS]"
    board_src = glob.glob(board_src, recursive=True)
    log.debug(f"> board  src = {board_src}")

    prefix = f"{build.prefix['build']}/bootloader/"
    include_list = [
        f"{build.prefix['bootloader']}/include/",
        f"{build.prefix['bootloader']}/boards/{build.target}/include/"
    ]
    return cmake_build_configure(
        prefix,
        {
            'name'    : 'bootloader',
            'version' : version,
            'linker'  : f"{build.prefix['bootloader']}/bootloader.ld",
            'ldflags' : compiles.ldflags,
            'cflags'  : compiles.cflags,
            'include' : include_list,
            'src'     : assets_src + common_src + board_src,
            'libs'    : compiles.libs,
            'toolchain_prefix' : compiles.toolchain_prefix,
            'toolchain_proc'   : compiles.toolchain_processor,
        }
    )
