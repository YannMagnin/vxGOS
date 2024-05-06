#include "vhex/kmalloc.h"
#include "vhex/hardware.h"

/* kmalloc_arena */
kmalloc_arena_t static_ram = { 0 };
kmalloc_arena_t vxboot_ram = { 0 };

#if 0
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
  then involve the "user" main() routine.
#endif


/* kmalloc_init() : initialize kmalloc module */
void kmalloc_init(void)
{
	extern uint32_t __sram_start;
	size_t size;

	static_ram.name = "_sram";
	static_ram.is_default = true;
	static_ram.start = &__sram_start;
	static_ram.end = (void*)(vhex[HWRAM_PHY_END] | 0x80000000);

	vxboot_ram.name = "_bram";
	vxboot_ram.is_default = false;
	vxboot_ram.start = (void*)vhex[HWRAM_PHY_USER_START];
	size = (vhex[HWRAM_PHY_USER_END] - vhex[HWRAM_PHY_USER_START]) / 2;
	vxboot_ram.end = (void*)(vhex[HWRAM_PHY_USER_START] + size);
	vxboot_ram.start = (void*)((uintptr_t)vxboot_ram.start | 0x80000000);
	vxboot_ram.end = (void*)((uintptr_t)vxboot_ram.end | 0x80000000);

	kmalloc_init_arena(&static_ram, true);
	kmalloc_init_arena(&vxboot_ram, true);
	kmalloc_add_arena(&static_ram);
	kmalloc_add_arena(&vxboot_ram);
}

/* kmalloc_quit() : quit the module */
void kmalloc_quit(void)
{
	/* Nothing to do here ? */
}
