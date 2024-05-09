//---
// devices:kterm:write  - write text to screen
//---

#include <stdarg.h>
#include <alloca.h>
#include <stdio.h>

#include "vhex/devices/kterm.h"
#include "vhex/_klibc.h"

//---
// Internals
//---

/* kterm object */
extern struct _kterm _kterm;

/* internal function */
extern int kterm_line_discipline(struct _kterm *kterm, char n);
extern int kterm_dascii(struct _kterm *kterm);

/* _kterm_display_string() : display string */
static int _kterm_display_string(
    struct _kterm *kterm,
    char const * const buffer
) {
    int i;

    for (i = 0 ; buffer[i] != '\0' ; ++i)
    {
        if (kterm_line_discipline(kterm, buffer[i]) != 0)
            continue;
        if (kterm_dascii(kterm) != 0)
            break;
    }
    return i;
}

//---
// Public
//---

/* kterm_write() : display a format string */
int kterm_write(char const * const format, ...)
{
    char *buffer;
    va_list ap;

    if (_kterm.__magic != _KTERM_INIT_MAGIC)
        return -1;
    buffer = alloca(128);
    if (buffer == NULL)
        return -2;
    va_start(ap, format);
    klibc_vsnprintf(buffer, 128, format, ap);
    va_end(ap);
    return _kterm_display_string(&_kterm, buffer);
}
