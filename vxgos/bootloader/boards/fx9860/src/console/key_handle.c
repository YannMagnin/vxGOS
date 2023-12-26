#include "bootloader/console.h"
#include "fx9860/keycode.h"

//---
// Public
//---

/* console_key_handle_special() : handle special key handling */
// TODO
// - F_UP   -> history
// - F_DOWN -> history
int console_key_handle_special(struct console *console, int key)
{
    switch (key)
    {
        case KEY_SHIFT:
            console->input.mode.shift ^= 1;
            return 1;
        case KEY_ALPHA:
            console->input.mode.maj ^= 1;
            return 1;
        case KEY_OPTN:
            console->input.mode.ctrl ^= 1;
            return 1;
        case KEY_DOT:
            console_buffer_in_insert(console, ' ');
            return 1;
        case KEY_DEL:
            console_buffer_in_remove(console);
            return 1;
        case KEY_EXE:
            console->input.mode.exit = 1;
            return 1;
        case KEY_LEFT:
            console_buffer_in_cursor_move(console, -1);
            return 1;
        case KEY_RIGHT:
            console_buffer_in_cursor_move(console, 1);
            return 1;
        default:
            return 0;
    }
}

/* console_key_handle(): Update the internal buffer with the given key code */
int console_key_handle(struct console *console, int key)
{
    static const uint8_t keylist_alpha[] = {
        KEY_XOT, KEY_LOG, KEY_LN, KEY_SIN, KEY_COS, KEY_TAN,
        KEY_FRAC, KEY_FD, KEY_LEFTP, KEY_RIGHTP, KEY_COMMA, KEY_ARROW,
        KEY_7, KEY_8, KEY_9,
        KEY_4, KEY_5, KEY_6, KEY_MUL, KEY_DIV,
        KEY_1, KEY_2, KEY_3, KEY_PLUS, KEY_MINUS,
        KEY_0, 0xff
    };
    static const uint8_t keylist_num[] = {
        KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
        KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
        KEY_PLUS, KEY_MINUS, KEY_MUL, KEY_DIV,
        KEY_LEFTP, KEY_RIGHTP, KEY_COMMA, KEY_POWER,
        KEY_DOT, KEY_FD, KEY_ARROW, 0xff
    };
    static const char keylist_num_char[] = "0123456789+-x/(),^.|_";
    const uint8_t *keycode_list;
    char character;
    int i;

    /* Get the appropriate key list */
    keycode_list = keylist_alpha;
    if (console->input.mode.shift == 1)
        keycode_list = keylist_num;

    /* Try to find the pressed key. */
    i = -1;
    while (keycode_list[++i] != 0xff && keycode_list[i] != key);
    if (keycode_list[i] != key)
        return 0;

    /* handle mode then update the buffer */
    if (console->input.mode.shift == 0) {
        character =  'a' + i;
        if (console->input.mode.maj == 1)
            character = 'A' + i;
    } else {
        character = keylist_num_char[i];
    }
    console_buffer_in_insert(console, character);
    return 1;
}
