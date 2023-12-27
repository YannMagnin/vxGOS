#include <criterion/criterion.h>

#include "bootloader/console.h"
#include "bootloader/display.h"

#include "utils.h"

//---
// Public
//---

/* console:open_close - check open and close primitives
 * @todo
 * - malloc fails
 * - console content */
Test(console, open_close)
{
    extern bool enable_font;
    extern bool enable_font_error;
    struct console console;

    enable_font = false;
    enable_font_error = false;
    _creq(console_open(NULL),     -1, "open console with no console");
    _creq(console_open(&console), -3, "open console with no font");

    enable_font_error = true;
    _creq(console_open(&console), -2, "open console with font error");
    enable_font_error = false;

    enable_font = true;
    _creq(console_open(&console),   0, "open console");

    _creq(console_close(NULL),     -1, "close with no console");
    _creq(console_close(&console),  0, "close console");
}
