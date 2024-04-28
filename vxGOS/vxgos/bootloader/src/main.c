#include "bootloader/console.h"
#include "bootloader/display.h"
#include "bootloader/bios.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

//---
// Public
//---

/* bootloader_main() : bootloader high-level entry */
int bootloader_main(
    uintptr_t   image_base_addr,
    size_t      image_size,
    uintptr_t   kernel_image_base_addr,
    size_t      kernel_image_size
) {
    struct console console;
    char buffer[128];

    //TODO : check special key pressed, if not simply boot the kernel

    if (console_open(&console) != 0)
        _bios_panic();
    console_write(
        &console,
        "vxGOS bootloader entry\n"
        "\n"
        "[+] primary bootloader info:\n"
        "    - image base  : %p\n"
        "    - image size  : %do\n"
        "    - kernel base : %p\n"
        "    - kernel size : %do\n"
        ,
        image_base_addr,
        image_size,
        kernel_image_base_addr,
        kernel_image_size
    );

    while (1) {
        console_write(&console, ">");
        console_read(&console, buffer, 128);
        if (buffer[0] == 'b' && buffer[1] == '\0')
            break;
        if (strcmp("boot", buffer) == 0)
            break;
    }

    console_write(&console, "booting kernel...\n");
    int ret = ((int(*)(uintptr_t,size_t))kernel_image_base_addr)(
        kernel_image_base_addr,
        kernel_image_size
    );
    console_write(&console, "%p\n", ret);
    console_write(&console, "waiting manual reset...");
    while(1) {
        __asm__ volatile ("sleep");
    }
}
