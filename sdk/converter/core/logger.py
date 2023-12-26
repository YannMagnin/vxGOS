"""
core.logger - Log wrapper
"""
import sys

__all__ = [
    'log'
]

#---
# Internals
#---

_LOG_DEBUG    = 7
_LOG_INFO     = 6
_LOG_NOTICE   = 5
_LOG_USER     = 4
_LOG_WARN     = 3
_LOG_ERR      = 2
_LOG_CRIT     = 1
_LOG_EMERG    = 0

class _VxLogger():
    def __init__(self, logfile=None):
        self._logfile = logfile
        self._level = _LOG_USER
        self._indent = 0

    #---
    # Internals
    #---

    def _print(self, level, text, skip_indent, fileno):
        if self._level < level:
            return 0
        if not skip_indent and self._level == _LOG_DEBUG and self._indent > 0:
            text = ('>>> ' * self._indent) + text
        print(text, file=fileno, end='', flush=True)
        return len(text) + 1

    #---
    # Public properties
    #---

    @property
    def level(self):
        """ <property> handle print level """
        return self._level

    @level.setter
    def level(self, level):
        """ <property> handle print level """
        if level < _LOG_EMERG or level > _LOG_DEBUG:
            print(f"[log] level update to {level} is not possible, ignored")
            return
        self._level = level

    @property
    def indent(self):
        """ <property> handle indentation level for __LOG_DEBUG """
        return self._indent

    @indent.setter
    def indent(self, indent):
        """ <property> handle indentation level for __LOG_DEBUG """
        if indent < 0:
            print(f"[log] indent update to {indent} is not possible, ignored")
            return
        self._indent = indent

    #---
    # Public methods
    #---

    def debug(self, text, end='\n', skip_indent=False):
        """ print debug log """
        return self._print(
            _LOG_DEBUG, f"[DEBUG] {text}{end}", skip_indent, sys.stdout
        )

    def info(self, text, end='\n', skip_indent=False):
        """ print info log """
        return self._print(
            _LOG_INFO, f"[INFO] {text}{end}", skip_indent, sys.stdout
        )

    def notice(self, text, end='\n', skip_indent=False):
        """ print notice log """
        return self._print(
            _LOG_NOTICE, f"[NOTICE] {text}{end}", skip_indent, sys.stdout
        )

    def user(self, text, end='\n', skip_indent=False):
        """ print user log """
        return self._print(_LOG_USER, f"{text}{end}", skip_indent, sys.stdout)

    def warn(self, text, end='\n', skip_indent=False):
        """ print warning log """
        return self._print(
            _LOG_WARN, f"[WARN] {text}{end}", skip_indent, sys.stderr
        )

    def error(self, text, end='\n', skip_indent=False):
        """ print error log """
        return self._print(
            _LOG_ERR, f"[ERROR] {text}{end}", skip_indent, sys.stderr
        )

    def critical(self, text, end='\n', skip_indent=False):
        """ print critical log """
        return self._print(
            _LOG_CRIT, f"[CRITICAL] {text}{end}", skip_indent, sys.stderr
        )

    def emergency(self, text, end='\n', skip_indent=False):
        """ print emergency log """
        self._print(
            _LOG_EMERG, f"[EMERGENCY] {text}{end}", skip_indent, sys.stderr
        )
        sys.exit(84)

#---
# Public functions
#---

log = _VxLogger()
