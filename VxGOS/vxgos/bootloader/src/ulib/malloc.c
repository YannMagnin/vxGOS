#include <stdlib.h>

#include "bootloader/bios.h"

//---
// Public
//---

/* malloc() : memory allocator */
void *malloc(size_t size)
{
    return _bios_malloc(size);
}
