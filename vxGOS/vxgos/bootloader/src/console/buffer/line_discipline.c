#include "bootloader/console.h"

//---
// Internals
//---

/* console_vertical_update() - Update vertical cursor */
static void console_vertical_update(
    struct console *console,
    unsigned int *y
) {
    if (*y + 1 < console->winsize.ws_col) {
        *y += 1;
    }
}

/* console_horizontal_update() - Update horizontal cursor */
static int console_horizontal_update(
    struct console *console,
    unsigned int *x,
    unsigned int *y
) {
    *x += 1;
    if (*x >= console->winsize.ws_col)
    {
        console_vertical_update(console, y);
        *x = 0;
        return 1;
    }
    return 0;
}

//---
// Public
//---

/* console_line_discipline() - Check "special" char */
int console_line_discipline(
    struct console *console,
    char n,
    unsigned int *x,
    unsigned int *y
) {
    int offset;

    switch (n)
    {
        case '\n':
            *x = 0;
            console_vertical_update(console, y);
            return 1;
        case '\b':
            if (*x > 0)
                *x -= 1;
            return 1;
        case '\v':
            console_vertical_update(console, y);
            return 1;
        case '\r':
            console->output.cursor.x = 0;
            return 1;
        case '\t':
            offset  = *x;
            offset -= ((*x / 5) * 5);
            offset  = 5 - offset;
            while (--offset >= 0)
                console_horizontal_update(console, x, y);
            return 1;
        default:
            console_horizontal_update(console, x, y);
            return 0;
    }
}
