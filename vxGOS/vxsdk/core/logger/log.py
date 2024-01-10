"""
vxsdk.core.logger.log   - logger object abstraction
"""
__all__ = [
    'log',
]
from typing import Any, Tuple, NoReturn
import sys
import inspect

#---
# Public
#---

class Logger():
    """ logger abstraction
    """
    LOG_DEBUG   = (7, 'DEBUG',     '\033[1;34m')
    LOG_INFO    = (6, 'INFO',      '\033[1;36m')
    LOG_NOTICE  = (5, 'NOTICE',    '\033[1m')
    LOG_USER    = (4, 'USER',      '\033[0m')
    LOG_WARN    = (3, 'WARNING',   '\033[1;33m')
    LOG_ERR     = (2, 'ERROR',     '\033[0;33m')
    LOG_CRIT    = (1, 'CRITICAL',  '\033[1;31m')
    LOG_EMERG   = (0, 'EMERGENCY', '\033[0;31m')

    def __init__(self) -> None:
        self._loglevel_limit    = Logger.LOG_USER
        self._indent_level      = 0
        self._indent_str        = '>>>>'

    def __call__(self, text: Any, **kwargs: Any) -> int:
        """ alias to log.user() """
        return self.user(text, **kwargs)

    #---
    # Internal methods
    #---

    def _print(
        self,
        level:    Tuple[int,str,str],
        text:     str,
        indent:   bool = True,
        fileno:   Any  = sys.stdout,
        **kwargs: Any,
    ) -> int:
        """ print() built-in wrapper with flush and indent handling
        """
        if self._loglevel_limit < level:
            return 0
        if level[0] != Logger.LOG_USER[0]:
            curframe = inspect.currentframe()
            calframe = inspect.getouterframes(curframe, 2)[2]
            text = \
                f"[{level[1]}]\n" \
                f"> at -> {calframe.filename}:{calframe.function}():" \
                f"{calframe.lineno}\n" \
                f"> context -> {text}"
        if indent and self._indent_level > 0:
            text = f"{self._indent_str * self._indent_level} {text}"
        print(f"{level[2]}{text}\033[0m", file=fileno, **kwargs)
        return len(text) + 1

    #---
    # Public methods
    #---

    def debug(self, text: Any, **kwargs: Any) -> int:
        """ print debug log """
        return self._print(Logger.LOG_DEBUG, text, **kwargs)

    def info(self, text: Any, **kwargs: Any) -> int:
        """ print info log """
        return self._print(Logger.LOG_INFO, text, **kwargs)

    def notice(self, text: Any, **kwargs: Any) -> int:
        """ print notice log """
        return self._print(Logger.LOG_NOTICE, text, **kwargs)

    def user(self, text: Any, **kwargs: Any) -> int:
        """ print user log """
        return self._print(Logger.LOG_USER, text, **kwargs)

    def warn(self, text: Any, **kwargs: Any) -> int:
        """ print warning log """
        return self._print(
            level   = Logger.LOG_WARN,
            fileno  = sys.stderr,
            text    = text,
            **kwargs,
        )

    def error(self, text: Any, **kwargs: Any) -> int:
        """ print error log """
        return self._print(
            level   = Logger.LOG_ERR,
            fileno  = sys.stderr,
            text    = text,
            **kwargs,
        )

    def critical(self, text: Any, **kwargs: Any) -> int:
        """ print critical log """
        return self._print(
            level   = Logger.LOG_CRIT,
            fileno  = sys.stderr,
            text    = text,
            **kwargs,
        )

    def emergency(self, text: Any, **kwargs: Any) -> NoReturn:
        """ print emergency log """
        self._print(
            level   = Logger.LOG_EMERG,
            fileno  = sys.stderr,
            text    = text,
            **kwargs,
        )
        sys.exit(84)

#---
# Public
#---

log = Logger()
