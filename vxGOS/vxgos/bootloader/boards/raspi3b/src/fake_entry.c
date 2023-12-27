#include <stdarg.h>
#include <stdlib.h>

#include "raspi3b/uart.h"
#include "raspi3b/mailbox.h"
#include "raspi3b/framebuffer.h"
#include "raspi3b/board.h"

#include "bootloader/display.h"

//---
// Public
//---

/* bootloader_main() : "real" bootloader entry */
extern int bootloader_main(uintptr_t image_base_addr, size_t image_size);

/* bootloader_fake_entry() : debug / fake bootloader entry */
void bootloader_fake_entry(uintptr_t image_base_addr, size_t image_size)
{

    uart_init();
    uart_puts("vxGOS fake bootloader entry!!\n\n");

    uart_puts("[+] bootstrap information:\n");
    uart_putf("- image base address = %p\n", image_base_addr);
    uart_putf("- image size         = %d\n", image_size);

    uart_puts("[+] initialize boards\n");
    board_init();

    uart_puts("[+] initilize framebuffer\n");
    framebuffer_init();
    framebuffer_debug();

    struct font *font;
    uart_puts("[+] font information:\n");
    if (dfont_get(&font) != 0) {
        uart_puts("- font not found :(\n");
    } else {
        uart_putf("- font struct = %p\n", font);
        uart_putf("- font data   = %p\n", font->data);
    }

    bootloader_main(image_base_addr, image_size);

    while (1) {
        __asm__ volatile ("nop");
    }
}
