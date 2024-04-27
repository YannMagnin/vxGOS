#include <stdbool.h>

#include "bootloader/display.h"

//---
// Public
//---

_Noreturn void _bios_panic(void)
{
    dclear(0xf100);
    dupdate();
    while (true) {
        __asm__ volatile ("sleep");
    }
}
