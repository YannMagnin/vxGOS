#include <stdbool.h>

#include "bootloader/display.h"

//---
// Public
//---

_Noreturn void _bios_panic(void)
{
    dclear(C_WHITE);
    dprint(0, 0, C_BLACK, "BIOS panic");
    dupdate();
    while (true) {
        __asm__ volatile ("sleep");
    }
}
