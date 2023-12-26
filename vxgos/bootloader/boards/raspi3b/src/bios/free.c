#include "bootloader/bios.h"
#include "raspi3b/memory.h"

//---
// Public
//---

/* _bios_free() : free allocated memory */
void _bios_free(void *ptr)
{
    memory_free(ptr);
}
