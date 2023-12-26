#include <string.h>
#include <stdio.h>

#include "bootloader/console.h"
#include "bootloader/display.h"

//---
// Public
//---

/* console_buffer_out_reset() : reset output buffer */
int console_buffer_out_reset(struct console *console)
{
    if (console == NULL)
        return -1;
    memset(console->output.buffer.data, 0x00, console->output.buffer.size);
    console->output.buffer.cursor = 0;
    return 0;
}

/* console_buffer_out_insert() : insert string anywhere in the output buffer */
int console_buffer_out_insert(struct console *console, char *buffer, size_t nb)
{
    size_t dump;
    void *start;

    /* check error */
    if (console == NULL)
        return -1;
    if (buffer == NULL)
        return -2;
    if (nb == 0)
        return 0;

    /* calculate the "real" number of byte to dump into the buffer */
    dump = nb;
    start = &buffer[0];
    if (dump > console->output.buffer.size) {
        start = &buffer[dump - console->output.buffer.size];
        dump  = console->output.buffer.size;
    }

    /* dump the buffer (be careful with the circular effect) */
    if (console->output.buffer.cursor + dump >= console->output.buffer.size)
    {
        memcpy(
            &console->output.buffer.data[console->output.buffer.cursor],
            start,
            console->output.buffer.size - console->output.buffer.cursor
        );
        dump -= console->output.buffer.size - console->output.buffer.cursor;
        console->output.buffer.cursor = 0;
    }
    memcpy(
        &console->output.buffer.data[console->output.buffer.cursor],
        start,
        dump
    );
    console->output.buffer.cursor += dump;
    return nb;
}

/* console_buffer_out_write() : display the output buffer on screen */
int console_buffer_out_display(struct console *console)
{
    uint8_t *buffer;
    size_t counter;
    int i;

    /* check obvious error */
    if (console == NULL)
        return -1;

    /* Due to potential special char, we should find the "real" starting
       index for the internal buffer */
    console->output.cursor.x = 0;
    console->output.cursor.y = 0;
    i = console->output.buffer.cursor;
    buffer = &console->output.buffer.data[0];
    counter = 0;
    while (1)
    {
        /* decrease the cursor and handle circular effect */
        if (--i < 0)
            i = console->output.buffer.size - 1;

        /* check loop condition */
        if (buffer[i] == '\0')
            break;

        /* update internal counter and check display limit */
        counter += 1;
        if (counter >= console->output.buffer.size_frame)
            break;

        /* handle the character (only to force update cursors) */
        console_line_discipline(
            console,
            buffer[i],
            &console->output.cursor.x,
            &console->output.cursor.y
        );
        if (console->output.cursor.y >= console->winsize.ws_row)
            break;
    }

    /* Display character per character because we need to check special
       behaviour (like cariege return, line feed, ...) */
    console->output.cursor.x = 0;
    console->output.cursor.y = 0;
    while (1)
    {
        /* update the index */
        if (++i >= (int)console->output.buffer.size)
            i = 0;
        if (i == (int)console->output.buffer.cursor)
            break;

        /* display part (character + cursor if needed) */
        if (
            console_line_discipline(
                console,
                buffer[i],
                &console->output.cursor.x,
                &console->output.cursor.y
            ) == 0
        ) {
            dascii(
                console->output.cursor.x * console->winsize.ft_xpixel,
                console->output.cursor.y * console->winsize.ft_ypixel,
                console->_private.color.fg,
                buffer[i],
                console->_private.font
            );
        }
    }
    return counter;
}
