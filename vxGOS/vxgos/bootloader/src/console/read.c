#include <string.h>

#include "bootloader/console.h"

//---
// Public
//---

/* console object */
extern struct console console;

/* console_read() : console read primitive */
int console_read(void *buffer, size_t nb)
{
    int error;
    int key;

    error = console_buffer_in_init(&console, buffer, nb);
    if (error < 0)
        return error;

    /* force display the empty buffer to draw the cursor line */
    console_buffer_in_display(&console);

    /* keyboard handling
     * @note
     * `console_key_*()` are provided externally by boards implementation */
    while (console.input.mode.exit == 0)
    {
        key = console_key_get();
        if (console_key_handle_special(&console, key) == 0)
            console_key_handle(&console, key);
        console_buffer_in_display(&console);
    }

    return console_buffer_in_uninit(&console);
}
