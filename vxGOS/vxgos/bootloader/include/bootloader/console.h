#ifndef BOOTLOADER_CONSOLE_H
#define BOOTLOADER_CONSOLE_H 1

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdarg.h>

#include "bootloader/display.h"

//---
// macros
//---

/* internal terminal hardcoded information */
#define TERM_BUFFER_NB_FRAME    (2)

//---
// Types
//---

/* define console structure */
struct console
{
    /* windows information */
    struct {
        size_t ws_col;
        size_t ws_row;
        size_t ws_xpixel;
        size_t ws_ypixel;
        size_t ft_xpixel;
        size_t ft_ypixel;
        size_t ft_csize;
    } winsize;

    /* output buffer information*/
    struct
    {
        struct {
            unsigned int x;
            unsigned int y;
        } cursor;
        struct {
            uint8_t *data;
            unsigned int cursor;
            size_t size;
            size_t size_frame;
        } buffer;
    } output;

    /* input buffer */
    struct {
        struct {
            unsigned int x;
            unsigned int y;
            unsigned int saved;
        } cursor;
        union {
            struct {
                uint8_t exit:   1;
                uint8_t ctrl:   1;
                uint8_t shift:  1;
                uint8_t maj:    1;
                uint8_t _res3:  1;
                uint8_t _res2:  1;
                uint8_t _res1:  1;
                uint8_t _res0:  1;
            } mode;
            uint8_t mode_raw;
        };
        struct {
            size_t max;
            size_t size;
            unsigned int cursor;
            char *data;
        } buffer;
    } input;

    /* private information */
    struct {
        struct {
            int fg;
            int bg;
        } color;
        struct font *font;
    } _private;
};

//---
// Public API
//---

/* console_open() : open console */
extern int console_open(void);

/* console_write() : printf-like write primitive (with line discipline) */
extern int console_write(char const *format, ...);

/* console_read() : console read primitive */
extern int console_read(void *buffer, size_t nb);

/* console_vwrite() : same as `console_write()` but with va_list */
extern int console_vwrite(const char *format, va_list ap);

/* console_close() : close console */
extern int console_close(void);


//---
// Low-level API
//---

// ouput buffer

/* console_buffer_out_reset() : reset output buffer */
extern int console_buffer_out_reset(struct console *console);

/* console_buffer_out_insert() : insert anywhere in the output buffer */
extern int console_buffer_out_insert(
    struct console *console,
    char *buffer,
    size_t nb
);

/* console_buffer_out_display() : display the output buffer on screen */
extern int console_buffer_out_display(struct console *console);

// input buffer

/* console_buffer_in_init() : initialize the input buffer */
extern int console_buffer_in_init(
    struct console *console,
    void *buffer,
    size_t n
);

/* console_buffer_in_uninit() : uninit input buffer and quit */
extern int console_buffer_in_uninit(struct console *console);

/* console_buffer_in_reset() : reset the input buffer */
extern void console_buffer_in_reset(struct console *console);

/* console_buffer_in_cursor_move() : move cursor in on direction */
extern void console_buffer_in_cursor_move(
    struct console *console,
    int direction
);

/* console_buffer_in_display() : display input buffer */
extern void console_buffer_in_display(struct console *console);

/* console_buffer_in_remove(): Remove character based on cursor position */
extern void console_buffer_in_remove(struct console *console);

/* console_buffer_in_insert() - Insert character at cursor position */
extern int console_buffer_in_insert(struct console *console, char n);

// line discipline

/* console_line_discipline() - Check "special" char */
extern int console_line_discipline(
    struct console *console,
    char n,
    unsigned int *x,
    unsigned int *y
);

// keyboard

/* console_key_get() : get current pressed key */
extern int console_key_get(void);

/* console_key_handle_special() : check if the key is the a special one */
extern int console_key_handle_special(struct console *console, int key);

/* console_key_handle() : handle current key */
extern int console_key_handle(struct console *console, int key);

#endif /* BOOTLOADER_CONSOLE_H */
