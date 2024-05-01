#include <string.h>
#include <stdlib.h>

#include "bootloader/console.h"
#include "bootloader/display.h"

//---
// Public
//---

/* declare the console object here */
struct console console;

/* console_open(): initialize and open the terminal */
int console_open(void)
{
    struct font *font;

    if (dfont_get(&font) != 0)
        return -2;
    if (font == NULL)
        return -3;

    memset(&console, 0x00, sizeof(struct console));
    dinfo(
        &console.winsize.ws_xpixel,
        &console.winsize.ws_ypixel
    );
    console.winsize.ft_xpixel = font->width + 1;
    console.winsize.ft_ypixel = font->height + 1;
    console.winsize.ws_col  = console.winsize.ws_xpixel;
    console.winsize.ws_col /= console.winsize.ft_xpixel;
    console.winsize.ws_row  = console.winsize.ws_ypixel;
    console.winsize.ws_row /= console.winsize.ft_ypixel;

    console.output.buffer.size_frame  = console.winsize.ws_row;
    console.output.buffer.size_frame *= console.winsize.ws_col;

    console.output.buffer.size  = console.output.buffer.size_frame;
    console.output.buffer.size *= TERM_BUFFER_NB_FRAME;
    console.output.buffer.size *= sizeof(uint8_t);

    console.output.buffer.data = calloc(
        sizeof(char),
        console.output.buffer.size
    );
    if (console.output.buffer.data == NULL) {
        console_close();
        return -4;
    }

    console._private.color.bg = C_BLACK;
    console._private.color.fg = C_WHITE;
    console._private.font = font;
    return 0;
}
