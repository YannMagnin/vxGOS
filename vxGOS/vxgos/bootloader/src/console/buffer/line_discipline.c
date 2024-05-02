#include "bootloader/console.h"

//---
// Internals
//---

/* console_vertical_update() - Update vertical cursor */
static int console_vertical_update(
    struct console *console,
    unsigned int *y
) {
    if (*y + 1 < console->winsize.ws_row) {
        *y += 1;
        return 1;
    }
    return -1;
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
        if (console_vertical_update(console, y) != 1)
            return -1;
        *x = 0;
    }
    return 1;
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
            return console_vertical_update(console, y);
        case '\b':
            if (*x > 0)
                *x -= 1;
            return 1;
        case '\v':
            return console_vertical_update(console, y);
        case '\r':
            console->output.cursor.x = 0;
            return 1;
        case '\t':
            offset  = *x;
            offset -= ((*x / 5) * 5);
            offset  = 5 - offset;
            while (--offset >= 0) {
                if (console_horizontal_update(console, x, y) != 1)
                    return -1;
            }
            return 1;
        default:
            console_horizontal_update(console, x, y);
            return 0;
    }
}
