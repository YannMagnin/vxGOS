#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <criterion/criterion.h>

#include "bootloader/console.h"
#include "utils.h"

//---
// Public
//---

/* buffer_in_insert() : check buffer input insert */
Test(console, buffer_in_insert)
{
    extern bool enable_font;
    extern bool enable_font_error;
    struct console console;
    char smallbuffer[5];

    /* init console */
    enable_font = true;
    enable_font_error = false;
    console_open(&console);

    /* check obvious error */
    _creq(console_buffer_in_init(NULL,     smallbuffer, 5), -1, "no console");
    _creq(console_buffer_in_init(&console,        NULL, 5), -2, "no buffer");
    _creq(console_buffer_in_init(&console, smallbuffer, 0), -3, "no size");
    _creq(console_buffer_in_uninit(NULL),                   -1, "no console");

    /* simple small injection */
    _creq(console_buffer_in_init(&console, smallbuffer, 5), 0, "buffer init");
    _creq(console_buffer_in_insert(&console, 'a'),  0, "small insert 0");
    _creq(console_buffer_in_insert(&console, 'b'),  0, "small insert 1");
    _creq(console_buffer_in_insert(&console, 'c'),  0, "small insert 2");
    _creq(console_buffer_in_insert(&console, 'd'), -1, "small insert 3");
    _creq(console_buffer_in_insert(&console, 'e'), -1, "small insert 4");
    _creq(console_buffer_in_insert(&console, 'f'), -1, "small insert 5");
    _creq(console_buffer_in_insert(&console, 'g'), -1, "small insert 6");
    _creq(console_buffer_in_insert(&console, 'h'), -1, "small insert 7");
    _creq(memcmp(console.input.buffer.data, "abc", 3), 0, "small insert");
    console_buffer_in_display(&console);
    _creq(console_buffer_in_uninit(&console), 4, "uninit error");

    /* small injection with moved cursor */
    _creq(console_buffer_in_init(&console, smallbuffer, 5), 0, "buffer init");
    console_buffer_in_reset(&console);
    _creq(console_buffer_in_insert(&console, 'a'),  0, "small insert at 0");
    console.input.buffer.cursor = 0;
    _creq(console_buffer_in_insert(&console, 'b'),  0, "small insert at 1");
    console.input.buffer.cursor = 1;
    _creq(console_buffer_in_insert(&console, 'c'),  0, "small insert at 2");
    console.input.buffer.cursor = 0;
    _creq(console_buffer_in_insert(&console, 'd'), -1, "small insert at 3");
    _creq(
        memcmp(console.input.buffer.data, "bca", 3), 0,
        "('%s' vs '%s')small insert",
        console.input.buffer.data,
        "bca"
    );
    console_buffer_in_display(&console);

    /* uninit check */
    _creq(console_buffer_in_uninit(&console), 4, "uninit error");

    /* exit */
     console_close(&console);
}

/* buffer_in_remove() : check buffer input remove */
Test(console, buffer_in_remove)
{
    extern bool enable_font;
    extern bool enable_font_error;
    struct console console;
    char smallbuffer[11];

    /* init console */
    enable_font = true;
    enable_font_error = false;
    console_open(&console);

    /* populate the input buffer */
    _creq(console_buffer_in_init(&console, smallbuffer, 11), 0, "buffer init");
    _creq(console_buffer_in_insert(&console, 'a'),  0, "small insert 0");
    _creq(console_buffer_in_insert(&console, 'b'),  0, "small insert 1");
    _creq(console_buffer_in_insert(&console, 'c'),  0, "small insert 2");
    _creq(console_buffer_in_insert(&console, 'd'),  0, "small insert 3");
    _creq(console_buffer_in_insert(&console, 'e'),  0, "small insert 4");
    _creq(console_buffer_in_insert(&console, 'f'),  0, "small insert 5");
    _creq(console_buffer_in_insert(&console, 'g'),  0, "small insert 6");
    _creq(console_buffer_in_insert(&console, 'h'),  0, "small insert 7");
    _creq(console_buffer_in_insert(&console, 'i'),  0, "small insert 8");

    /* try to remove all char */
    _creq(memcmp(console.input.buffer.data, "abcdefghi", 10), 0, "small rm0");
    console_buffer_in_cursor_move(&console, -1);
    console_buffer_in_cursor_move(&console, -1);
    console_buffer_in_cursor_move(&console, -1);
    console_buffer_in_remove(&console);
    _creq(
        memcmp(console.input.buffer.data, "abcdeghi",  9), 0,
        "small rm1 (%s vs %s)",
        console.input.buffer.data,
        "abcdeghi"
    );
    console_buffer_in_remove(&console);
    _creq(memcmp(console.input.buffer.data, "abcdghi",   8), 0, "small rm2");
    console_buffer_in_remove(&console);
    _creq(memcmp(console.input.buffer.data, "abcghi",    7), 0, "small rm3");
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    console_buffer_in_remove(&console);
    _creq(
        memcmp(console.input.buffer.data, "ghi",  4), 0,
        "small rm1 (%s vs %s)",
        console.input.buffer.data,
        "ghi"
    );

    /* re-populate input buffer */
    _creq(console_buffer_in_init(&console, smallbuffer, 5), 0, "buffer init");
    _creq(console_buffer_in_insert(&console, 'a'),  0, "small insert 0");
    _creq(console_buffer_in_insert(&console, 'b'),  0, "small insert 1");
    _creq(console_buffer_in_insert(&console, 'c'),  0, "small insert 2");

    /* code coverage cursor move */
    _creq(console.input.buffer.cursor, 3, "cursor check 0");
    console_buffer_in_cursor_move(&console, -1);
    _creq(console.input.buffer.cursor, 2, "cursor check 1");
    console_buffer_in_cursor_move(&console, -1);
    _creq(console.input.buffer.cursor, 1, "cursor check 2");
    console_buffer_in_cursor_move(&console, -1);
    _creq(console.input.buffer.cursor, 0, "cursor check 3");
    console_buffer_in_cursor_move(&console, -1);
    _creq(console.input.buffer.cursor, 0, "cursor check 4");
    console_buffer_in_cursor_move(&console, 1);
    _creq(console.input.buffer.cursor, 1, "cursor check 4");
    console_buffer_in_cursor_move(&console, 1);
    _creq(console.input.buffer.cursor, 2, "cursor check 4");
    console_buffer_in_cursor_move(&console, 1);
    _creq(console.input.buffer.cursor, 3, "cursor check 4");
    console_buffer_in_cursor_move(&console, 1);
    _creq(console.input.buffer.cursor, 3, "cursor check 4");

    /* try to remove with cursor position modification */
    _creq(memcmp(console.input.buffer.data, "abc", 4), 0, "small remove 0");
    console.input.buffer.cursor = 1;
    console_buffer_in_remove(&console);
    _creq(memcmp(console.input.buffer.data, "bc",  3), 0, "small remove 1");
    console.input.buffer.cursor = 0;
    console_buffer_in_remove(&console);
    console.input.buffer.cursor = 1;
    console_buffer_in_remove(&console);
    _creq(memcmp(console.input.buffer.data, "c",   2), 0, "small remove 2");
    console.input.buffer.cursor = 1;
    console_buffer_in_remove(&console);
    _creq(memcmp(console.input.buffer.data, "\0",  1), 0, "small remove 3");

    /* uninit check */
    _creq(console_buffer_in_uninit(&console), 1, "uninit error");
     console_close(&console);
}
