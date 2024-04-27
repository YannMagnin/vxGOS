//---
// fxcg50:initialize - Kernel initialization and C runtime
//---

#include "vhex/hardware.h"
#include "vhex/kmalloc.h"
#include "vhex/kernel.h"

#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>


/* Jumping there will properly unwind and leave the kernel */
jmp_buf vhex_exitbuf;

/* Return value of main() */
int vhex_exitcode;

/* initialize() : Where it all starts

    We are currently in a RAM location at a non-constant address due to the
  relocalization performed by the bootloader (vxBoot). Moreover, we are in
  privileged mode with one golden rule:

         Do not disturb the operating system

    Why we should keep this rule in mind? Because, on this particular
  device, none of all hardware and software information are documented.
  Vhex will therefore involve Casio's syscall to perform some interaction,
  especially concerning file manipulation.

    Note that we don't need to change the stack, the kernel will use the
  one given by Casio because it's big enough to store the user-application
  (mainly the vxOS) and the kernel code. However, we need to keep in mind
  that for this device, a lot of hardware supposition will be performed
  here, chiefly concerning the memory discovery.

               +---------------+
               |               |
               |               |
               | Reserved Area |
               |               |
               |               |
               + - - - - - - - + <-- virtually mapped at 0x0810000
               |               |
               |     vxBoot    | <-- This part will be re-used by kmalloc
               |               |
               +---------------+ <-- variable
               |               |
               |     Stack     |
               |               |
               + - - - - - - - + <-- end of the TLB information (variable)
               |    vxKernel   |
               |       +       |
               |      VxOS     |
               +---------------+ <-- Provided by the vxKernel (variable)
               |               |
               |               |
               |               |
               |   kmalloc()   |
               |               |
               |               |
               |               |
               +---------------+ <-- variable

    For the memory discovery. This an "early" exotic step for the vxkernel
  because we need to detect the RAM geometry to configure the "kmalloc"
  module (which provide memory management API like kmalloc(), kfree(),
  ...). For more information about this part is in
  <board/fxcg50/hardware.c>

    The function initialize is the "real" first code executed by the
  kernel. Its role is to perform some hardware detection, create and
  initialize the hypervisor, bootstrap the vxkernel world (see
  <include/hypervisor.h> for details) which will initialize the device,
  then involve the "user" main() routine. */
void initialize(void)
{
    /* Detect hardware information : This part is important because it will
       tell us if we are running in an emulator or on a real device. */
    hw_detect();

    /* Install Vhex, switch VBR, initialize drivers and modules */
    kinit();

    /* We are now running on our own in kernel mode. Since we have taken
       control of interrupts, pretty much any interaction with the system
       will break it. We'll limit our use of syscalls and do device driving
       ourselves. (Hopefully we can add cool features in the process!) */

    /* Now that we have initialized the kernel, we are ready to start the
       hosted user application, which has its own constructors and
       destructors to work with. */

    /* Here, we use exit() to allow the standard library to do
       what it wants in exit() after main() finishes executing */

    extern int kernel_debug(void);
    kernel_debug();

    //if(setjmp(vhex_exitbuf) == 0) {
    //    exit(main());
    //}

    /* Before leaving the kernel (which simply wait a manual reset),
       we need to clean everything we changed to hardware settings and
       peripheral modules. The OS is bound to be confused (and hang, or
       crash, or any other kind of giving up) if we don't restore them. */

    /* Unload vhex and give back control to the system. Driver settings
       will be restored while interrupts are disabled */
    kquit();

    /* deep sleep */
    //TODO: configure SLEEP
    while (1) {
        __asm__ volatile ("sleep":::);
    }
}
