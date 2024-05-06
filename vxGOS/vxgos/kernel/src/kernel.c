//---
// kernel - Kernel initialization and C runtime
//---

#include <stdbool.h>

#include "vhex/devices/kterm.h"
#include "vhex/kernel.h"

//---
// Public
//---

/* kernel_initialize() : Where it all starts
 *
 * We are currently in a RAM location at a non-constant address due to the
 * relocalization performed by the bootloader. Moreover, we are in privileged
 * mode and all drivers are in instable-state (not configured or partialy
 * configured on Casio's device).
 *
 * This routine will initialise an "early terminal" (kterm) which will allow
 * us to graphicaly display information before (in quind of sort) any kernel
 * initialisation (no buffer, no read support, ...)
 *
 * (todo) : after kterm */
void kernel_initialize(void)
{
    if (kterm_init() != 0)
        kernel_panic(NULL);
    //kterm_write("kterm initialised...\n");

    while (true) {
        __asm__ volatile ("sleep");
        //kernel_panic("kernel want to return o(x_x)o\n");
    }
}
