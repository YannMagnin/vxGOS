#include "bootloader/bios.h"
#include "raspi3b/memory.h"

//---
// Public
//---

/* _bios_malloc() : memory allocator */
void *_bios_malloc(size_t size)
{
    return memory_alloc(
        size,
        4,
        MEM_FLAG_NO_INIT | MEM_FLAG_NORMAL
    );
}
