#include <stdlib.h>
#include <stdio.h>
#include <criterion/criterion.h>

#include "bootloader/console.h"
#include "utils.h"

//---
// Public
//---

/* buffer_out_fonctional() : check buffer output behaviour */
Test(console, buffer_out_fonctional)
{
    extern bool enable_font;
    extern bool enable_font_error;
    struct console console;
    char *buffer;
    size_t size;

    /* init console */
    enable_font = true;
    enable_font_error = false;
    console_open(&console);

    /* check argument error */
    _creq(console_buffer_out_insert(NULL, "aaa", 3), -1, "no console");
    _creq(console_buffer_out_insert(&console, "aaa", 0), 0, "empty buffer");
    _creq(console_buffer_out_insert(&console,  NULL, 3), -2, "null buffer");
    _creq(console_buffer_out_display(NULL), -1, "no console");
    _creq(console_buffer_out_reset(NULL), -1, "no console");

    /* check simple insertion */
    console_buffer_out_insert(&console, "abcdefghij", 10);
    _creq(
        memcmp(console.output.buffer.data, "abcdefghij", 10),
        0,
        "simple injection error"
    );

    /* check multiple injection */
    console_buffer_out_reset(&console);
    console_buffer_out_insert(&console, "abcdefghij", 10);
    console_buffer_out_insert(&console, "klnmopqrst", 10);
    _creq(
        memcmp(console.output.buffer.data, "abcdefghijklnmopqrst", 20),
        0,
        "multiple injection error"
    );

    /* check long injection (buffer limit) */
    console_buffer_out_reset(&console);
    buffer = malloc(console.output.buffer.size);
    _creq(buffer != NULL, 1, "[internal] unable to load buffer");
    memset(buffer, 'a', console.output.buffer.size);
    console_buffer_out_insert(&console, buffer, console.output.buffer.size);
    _creq(
        memcmp(
            console.output.buffer.data, buffer, console.output.buffer.size
        ),
        0,
        "long injection (limit) fail"
    );
    console_buffer_out_display(&console);
    free(buffer);

    /* too long insertion */
    console_buffer_out_reset(&console);
    size  = console.output.buffer.size;
    size += console.output.buffer.size / 2;
    buffer = malloc(size);
    _creq(buffer != NULL, 1, "[internal] unable to load buffer");
    memset(&buffer[0], 'a', console.output.buffer.size / 2);
    memset(
        &buffer[console.output.buffer.size / 2],
        'b',
        console.output.buffer.size
    );
    console_buffer_out_insert(&console, buffer, size);
    _creq(
        memcmp(
            console.output.buffer.data,
            &buffer[console.output.buffer.size / 2],
            console.output.buffer.size
        ),
        0,
        "too long insert error"
    );
    console_buffer_out_display(&console);
    free(buffer);

    /* close console and exit */
    console_close(&console);
}

/* buffer_out_converage() : dummy code coverage check */
//FIXME : proper register screen characters available
Test(console, buffer_out_coverage)
{
    extern bool enable_font;
    extern bool enable_font_error;
    struct console console;
    char *buffer;
    size_t size;

    enable_font = true;
    enable_font_error = false;
    console_open(&console);

    /* check empty buffer draw */
    _creq(console_buffer_out_display(&console), 0, "empty buffer");

    /* check multiple insertion + newline char + special char */
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "aaaa\n", 5);
    console_buffer_out_insert(&console, "\t\v\b\r", 4);
    _creq(console_buffer_out_display(&console), 69, "newlines + special");

    /* cursor handling + multiple insertion */
    size = console.output.buffer.size / 2;
    buffer = malloc(size);
    memset(buffer, 'a', size);
    console_buffer_out_reset(&console);
    console_buffer_out_insert(&console, buffer, size);
    _creq(
        console_buffer_out_display(&console),
        console.output.buffer.size_frame,
        "cursor frame size check"
    );
    console_buffer_out_insert(&console, buffer, size);
    _creq(
        console_buffer_out_display(&console),
        console.output.buffer.size_frame,
        "multiple cursor check"
    );

    /* check vertical break with vertical tab */
    memset(buffer, '\v', size);
    console_buffer_out_reset(&console);
    console_buffer_out_insert(&console, buffer, size);
    _creq(
        console_buffer_out_display(&console),
        console.winsize.ws_row,
        "vertical tab limit"
    );

    /* long buffer insertion */
    size *= 4;
    buffer = realloc(buffer, size);
    memset(buffer, 'a', size);
    console_buffer_out_reset(&console);
    console_buffer_out_insert(&console, buffer, size);
    _creq(
        console_buffer_out_display(&console),
        console.output.buffer.size_frame,
        "long buffer check"
    );

    console_close(&console);
    free(buffer);
}
