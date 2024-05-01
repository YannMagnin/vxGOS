#include <string.h>

#include "bootloader/console.h"
#include "bootloader/display.h"

//---
// Public
//---

/* console_buffer_in_init() : initialize the input buffer */
int console_buffer_in_init(struct console *console, void *buffer, size_t nb)
{
    if (console == NULL)
        return -1;
    if (buffer == NULL)
        return -2;
    if (nb < 2)
        return -3;
    console->input.cursor.saved = console->output.buffer.cursor;
    console->input.cursor.x = console->output.cursor.x;
    console->input.cursor.y = console->output.cursor.y;
    console->input.buffer.data = buffer;
    console->input.buffer.size = 0;
    console->input.buffer.max = nb;
    console->input.buffer.cursor = 0;
    console->input.mode_raw = 0x00;
    return 0;
}

/* console_buffer_in_uninit() : uninit input buffer and quit */
int console_buffer_in_uninit(struct console *console)
{
    int readed;

    /* check error */
    if (console == NULL)
        return -1;

    /* inject the new-line and the EOL */
    console->input.buffer.data[console->input.buffer.size] = '\n';
    console->input.buffer.size += 1;
    console->input.buffer.data[console->input.buffer.size] = '\0';

    /* insert the input buffer */
    console_buffer_out_insert(
        console,
        console->input.buffer.data,
        console->input.buffer.size
    );

    /* remove the new-line before the EOL to have the same behaviour than
     * readline() */
    console->input.buffer.size -= 1;
    console->input.buffer.data[console->input.buffer.size] = '\0';

    /* reset buffer information and quit */
    readed = console->input.buffer.size;
    console->input.buffer.data = NULL;
    console->input.buffer.size = 0;
    console->input.buffer.max = 0;
    return readed;
}

/* console_buffer_in_reset() : reset the input buffer */
void console_buffer_in_reset(struct console *console)
{
    console->input.buffer.cursor = 0;
    console->input.buffer.size = 0;
}

/* console_buffer_in_cursor_move() : move cursor in on direction */
void console_buffer_in_cursor_move(struct console *console, int direction)
{
    if (direction < 0) {
        if (console->input.buffer.cursor > 0)
            console->input.buffer.cursor -= 1;
    } else {
        if (console->input.buffer.cursor < console->input.buffer.size)
            console->input.buffer.cursor += 1;
    }
}

/* console_buffer_in_display() : display input buffer */
void console_buffer_in_display(struct console *console)
{
    unsigned int x;
    unsigned int y;

    /* precalculate the cursor drawing position */
    x = console->input.cursor.x + 1;
    y = console->input.cursor.y;
    for (size_t i = 0 ; i < console->input.buffer.cursor ; ++i)
    {
        console_line_discipline(
            console,
            console->input.buffer.data[i],
            &x,
            &y
        );
    }
    x *= console->winsize.ft_xpixel;
    y *= console->winsize.ft_ypixel;

    /* display management */
    dclear(console->_private.color.bg);
    console_buffer_out_display(console);
    for (size_t i = 0; i < console->input.buffer.size ; ++i)
    {
        if (
            console_line_discipline(
                console,
                console->input.buffer.data[i],
                &console->output.cursor.x,
                &console->output.cursor.y
            ) == 0
        ) {
            dascii(
                console->output.cursor.x * console->winsize.ft_xpixel,
                console->output.cursor.y * console->winsize.ft_ypixel,
                console->_private.color.fg,
                console->input.buffer.data[i],
                console->_private.font
            );
        }
    }
    dline(
        x, y,
        x, y + console->winsize.ft_ypixel,
        console->_private.color.fg
    );
    dupdate();
}

/* console_buffer_in_remove(): Remove character based on cursor position */
void console_buffer_in_remove(struct console *console)
{
    /* check if this action is possible */
    if (console->input.buffer.cursor == 0)
        return;

    /* move data if needed */
    if (console->input.buffer.cursor < console->input.buffer.size)
    {
        for (
            size_t i = console->input.buffer.cursor - 1;
            i < console->input.buffer.size - 1;
            i++
        ) {
            console->input.buffer.data[i] = \
                console->input.buffer.data[i + 1];
        }
    }

    /* force NULL-char and update cursor/size */
    console->input.buffer.size   -= 1;
    console->input.buffer.cursor -= 1;
    console->input.buffer.data[console->input.buffer.size] = '\0';
}

/* console_buffer_in_insert() - Insert character based on cursor position */
int console_buffer_in_insert(struct console *console, char n)
{
    unsigned int i;

    /* save space for the "\n\0" (EOL) */
    if (console->input.buffer.size + 2 >= console->input.buffer.max)
        return -1;

    /* move data if needed */
    if (console->input.buffer.cursor < console->input.buffer.size) {
        i = console->input.buffer.size + 1;
        while (--i > console->input.buffer.cursor) {
            console->input.buffer.data[i] = \
                console->input.buffer.data[i - 1];
        }
    }

    /* insert the character and force-update all */
    console->input.buffer.data[console->input.buffer.cursor] = n;
    console->input.buffer.cursor += 1;
    console->input.buffer.size += 1;

    /* force NULL char */
    console->input.buffer.data[console->input.buffer.size] = '\0';
    return 0;
}
