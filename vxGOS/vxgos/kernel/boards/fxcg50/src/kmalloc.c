#include "vhex/kmalloc.h"
#include "vhex/hardware.h"

/* kmalloc_arena */
kmalloc_arena_t static_ram = { 0 };
kmalloc_arena_t vxboot_ram = { 0 };


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
