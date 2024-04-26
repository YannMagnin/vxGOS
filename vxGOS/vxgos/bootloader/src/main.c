#include "bootloader/console.h"
#include "bootloader/display.h"
#include "bootloader/bios.h"

#include <stdint.h>
#include <stddef.h>

//---
// Public
//---

/* bootloader_main() : bootloader high-level entry */
int bootloader_main(uintptr_t image_base_addr, size_t image_size)
{
    struct console console;
    char buffer[128];

    //TODO : check special key pressed, if not simply boot the kernel

    if (console_open(&console) != 0)
        _bios_panic();
    console_write(
        &console,
        "vxGOS bootloader entry\n"
        "\n"
        "[+] primary bootloader information:\n"
        "    - image base : %p\n"
        "    - image size : %do\n"
        ,
        image_base_addr,
        image_size
    );

    while (1) {
        console_write(&console, ">");
        console_read(&console, buffer, 128);
    }
}
