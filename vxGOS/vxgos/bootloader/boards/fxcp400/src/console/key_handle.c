#include <stdbool.h>

#include "bootloader/console.h"
#include "fxcp400/keycode.h"

//---
// Public
//---

/* console_key_handle_special() : handle special key handling
 * @note
 * - KEY_SHIFT  -> switch between alphabetical or numeric layout
 * - KEY_
 * */
// TODO
// - F_UP   -> history
// - F_DOWN -> history
int console_key_handle_special(struct console *console, int key)
{
    switch (key)
    {
        // modding

        case KEY_KEYBOARD:
            console->input.mode.shift ^= 1;
            return 1;
        case KEY_MAJ:
            console->input.mode.maj ^= 1;
            return 1;

        // special char

        case KEY_SPACE:
            console_buffer_in_insert(console, ' ');
            return 1;
        case KEY_DELETE:
            console_buffer_in_remove(console);
            return 1;
        case KEY_ENTER:
            console->input.mode.exit = 1;
            return 1;

        // navigation

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

/* console_key_handle(): Update the internal buffer with the given key */
// (todo) : missing letter "y" and "z" because we do not have enough keys
int console_key_handle(struct console *console, int key)
{
    static const uint8_t keylist_alpha[] = {
        KEY_EQUAL,  KEY_X, KEY_Y,   KEY_Z,     KEY_POWER, KEY_DIV,
        KEY_LEFTP,  KEY_7, KEY_8,   KEY_9,     KEY_MUL,
        KEY_RIGHTP, KEY_4, KEY_5,   KEY_6,     KEY_MINUS,
        KEY_COMMA,  KEY_1, KEY_2,   KEY_3,     KEY_PLUS,
        KEY_NEG,    KEY_0, KEY_EXP, KEY_CLEAR, // (todo) "z"
        KEY_UNUSED,
    };
    static const uint8_t keylist_num[] = {
        KEY_0,      KEY_1,     KEY_2,     KEY_3,   KEY_4,
        KEY_5,      KEY_6,     KEY_7,     KEY_8,   KEY_9,
        KEY_PLUS,   KEY_MINUS, KEY_MUL,   KEY_DIV, KEY_LEFTP,
        KEY_RIGHTP, KEY_COMMA, KEY_POWER, KEY_DOT, KEY_Z,
        KEY_Y,
        KEY_UNUSED,
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
    i = 0;
    while (true)
    {
        if (keycode_list[i] == KEY_UNUSED)
            break;
        if (keycode_list[i] == key)
            break;
        i += 1;
    }
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
