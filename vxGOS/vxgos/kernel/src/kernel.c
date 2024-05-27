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
void kernel_initialize(uintptr_t kernel_base_address, size_t kernel_size)
{
    if (kterm_init() != 0)
        kernel_panic(NULL);
    kterm_write("welcome to vxGOS!\n");
    kterm_write("\n");
    kterm_write("[+] kterm initialised\n");
    kterm_write("[+] kernel information:\n");
    kterm_write("  - kernel base address\t%p\n", kernel_base_address);
    kterm_write("  - kernel size\t\t\t%d\n", kernel_size);
    kterm_write("[+] initialising the kernel allocator...\n");
    // (todo) : kernel allocator
    kterm_write("[+] early 'board' module loading...\n");
    // (todo) : load board-specific module
    kterm_write("[+] hypervisor bootstrapping...\n");
    // (todo) : save the current world and install all driver

#if 0
    int counter = 0;
    while (true)
    {
        kterm_write("- test scroll %d\n", counter);
        for (int i = 0 ; i < 99999999 ; ++i) {
            __asm__ volatile ("nop");
        }
        counter += 1;
    }
#endif

    while (true) {
        __asm__ volatile ("sleep");
        //kernel_panic("kernel want to return o(x_x)o\n");
    }
}
