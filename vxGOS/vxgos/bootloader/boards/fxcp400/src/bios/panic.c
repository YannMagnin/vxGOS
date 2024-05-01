#include <stdbool.h>

#include "bootloader/display.h"

//---
// Public
//---

/* _bios_panic() : BIOS panic screen */
_Noreturn void _bios_panic(void)
{
    dclear(0xf100);
    dprint(0, 0, C_WHITE, "BIOS panic");
    dupdate();
    while (true) {
        __asm__ volatile ("sleep");
    }
}
