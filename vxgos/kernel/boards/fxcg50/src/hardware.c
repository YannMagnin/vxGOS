//---
//	vhex:core:hardware - Platform information and hardware detection
//---
#include "vhex/driver/mpu/sh/sh7305/pfc.h"
#include "vhex/driver/mmu.h"
#include "boards/fxcg50/include/hardware.h"
#include "vhex/hardware.h"


/* Holds information about the current platform */
uintptr_t vhex[HW_KEYS];

/* Processor Version Register */
#define PVR (*((volatile uint32_t *)0xff000030))
/* Product Register */
#define PRR (*((volatile uint32_t *)0xff000044))



/* ram_get_size() : try to determine the RAM size */
static ptrdiff_t ram_get_size(uintptr_t start_ram)
{
	volatile uint32_t *ptr = (void*)start_ram;
	volatile uint32_t *tst = (void*)start_ram;
	uintptr_t backup;

	//TODO: use UTLB page size information to walk through each pages
	while (1) {
		ptr = &ptr[4096];
		backup = ptr[0];
		ptr[0] = 0xdeadbeef;
		if (ptr[0] != 0xdeadbeef || ptr[0] == tst[0]) break;
		ptr[0] = backup;
	}
	ptr[0] = backup;
	return ((ptrdiff_t)((uintptr_t)ptr - (uintptr_t)tst));
}

/* hw_detect(): Basic hardware detection */
void hw_detect(void)
{
	/* get MPU and CPU information */
	vhex[HWMPU] = HWMPU_SH7305;
	vhex[HWCPUVR] = PVR;
	vhex[HWCPUPR] = PRR;

	/* We can differentiate the Prizm device from the fxcg50 device by
	   checking its stack */
	uintptr_t stack;
	__asm__("mov r15, %0" : "=r"(stack));

	/* detect the device type */
	vhex[HWDEVICE] = HWDEVICE_FXCG50;
	if (stack < 0x8c000000)
		vhex[HWDEVICE] = HWDEVICE_PRIZM;
	if (PVR == 0x00000000)
		vhex[HWDEVICE] = HWDEVICE_FXCG_MANAGER;

	/* detect the RAM geometry
	     We know that the "user RAM" is *constantly* mapped at 0x08100000 in
	   any Casio's calculator and all the memory reserved for the user is
	   completely loaded and stored at the end of the UTLB.

	     Here, we walk throuth the UTLB information and try to find the
	   physical address of the "user RAM". With this, we can determine the
	   starting address of the physical RAM[1] then the size of the RAM.

	   [1] : this part is a hardware speculation concerning the BSC (Bus
	           State Controler) which provide the physical address map and
	           can fiably give us the physical address of the RAM. But, on
	           the SH7305, the module is still undocumented ; I have
	           performed some reverse in the emulator to find information
	           about this module and I found that the RAM starting address
	           can be guest with the mask 0xff000000 and that the RAM area
	           is the Area 3. */
	uintptr_t uram = mmu_translate(0x08100000, NULL);
	vhex[HWRAM_PHY_USER_START] = uram;
	vhex[HWRAM_PHY_USER_END] = uram + mmu_uram_size();
	vhex[HWRAM_PHY_ORIGIN] = uram & 0xff000000;
	vhex[HWRAM_PHY_SIZE] = ram_get_size(uram | 0xa0000000);
	vhex[HWRAM_PHY_END] = vhex[HWRAM_PHY_ORIGIN] + vhex[HWRAM_PHY_SIZE];
}
