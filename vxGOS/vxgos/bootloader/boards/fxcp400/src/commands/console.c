#include "bootloader/command.h"
#include "bootloader/console.h"

//---
// Internals
//---

/* _command_console() : console debuger */
static int _command_console(void)
{
    extern struct console console;

    console_write("WS_COL\t= %d\n", console.winsize.ws_col);
    console_write("WS_ROW\t= %d\n", console.winsize.ws_row);
    console_write("WS_XPIXEL\t= %d\n", console.winsize.ws_xpixel);
    console_write("WS_YPIXEL\t= %d\n", console.winsize.ws_ypixel);
    console_write("FT_XPIXEL\t= %d\n", console.winsize.ft_xpixel);
    console_write("FT_YPIXEL\t= %d\n", console.winsize.ft_ypixel);
    return 0;
}

/* declare command "c" */
VCMD_DECLARE(
    cmd_console,
    .name   = "c",
    .desc   = "console debug",
    .func   = (void *)&_command_console
);
