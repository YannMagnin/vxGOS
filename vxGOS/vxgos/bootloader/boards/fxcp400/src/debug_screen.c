#include <stdbool.h>

#include "bootloader/display.h"
#include "bootloader/bios.h"

#include "bootloader/console.h"

void debug_main(void)
{
    struct console console;

    dclear(0x001f);
    dupdate();

    console_open(&console);
    //console_write(&console, "test");

    //dclear(0x07e0);
    //dupdate();

    while (true) {
        __asm__ volatile ("sleep");
    }
}
