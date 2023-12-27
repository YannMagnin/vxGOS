#include <stdarg.h>
#include <stdio.h>

#include "bootloader/console.h"
#include "bootloader/display.h"

//---
// Public
//---

/* console_vwrite() : printf wrapper for the terminal device with va_list */
int console_vwrite(struct console *console, const char *format, va_list ap)
{
    char buffer[1024];
    int nb;

    /* process the format */
    nb = vsnprintf(buffer, 1024, format, ap);

    /* update the internal buffer */
    console_buffer_out_insert(console, buffer, nb);

    /* display the internal buffer */
    dclear(console->_private.color.bg);
    console_buffer_out_display(console);
    dupdate();
    return nb;
}

/* console_write() - printf wrapper for the terminal device */
int console_write(struct console *console, char const *format, ...)
{
    va_list ap;
    int nb;

    va_start(ap, format);
    nb = console_vwrite(console, format, ap);
    va_end(ap);

    return nb;
}
