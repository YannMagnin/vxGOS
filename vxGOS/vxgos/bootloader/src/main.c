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
    char buffer[128];
    int ret;

    //TODO : check special key pressed, if not simply boot the kernel

    if (console_open() != 0)
        _bios_panic();
    console_write(
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

    ret = 0;
    while (1) {
        if (ret == 0) {
            console_write(">");
        } else {
            console_write("[%d]>", ret);
        }
        console_read(buffer, 128);
        if (buffer[0] == 'b' && buffer[1] == '\0')
            break;
        if (strcmp("boot", buffer) == 0)
            break;
        ret = command_exec(buffer);
    }

    console_write("booting kernel...\n");
    ret = ((int(*)(uintptr_t,size_t))kernel_image_base_addr)(
        kernel_image_base_addr,
        kernel_image_size
    );
    console_write("%p\n", ret);
    console_write("waiting manual reset...");
    while(1) {
        __asm__ volatile ("sleep");
    }
}
