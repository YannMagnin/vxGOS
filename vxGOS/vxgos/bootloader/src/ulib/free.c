#include "bootloader/bios.h"

//---
// Public
//---

/* free() : free allocated memory */
void free(void *ptr)
{
    return _bios_free(ptr);
}
