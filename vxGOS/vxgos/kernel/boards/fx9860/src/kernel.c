//---
// fx9860:panic     - kernel panic
//---

#include <stdbool.h>

#include "vhex/devices/kterm.h"

//---
// Public
//---

/* kernel_panic() : kernel panic display */
_Noreturn void kernel_panic(char const * const text)
{
    if (text != NULL) {
        kterm_clear(0xffff);
        kterm_write(text);
    }
    while (true) {
        __asm__ volatile ("sleep");
    }
}
