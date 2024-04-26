#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "bootloader/bios.h"

//---
// Internals
//---

/* __casio_malloc() : malloc wrapper */
static void* __casio_malloc = NULL;

//---
// Public
//---

/* _bios_malloc() : malloc wrapper */
void *_bios_malloc(size_t size)
{
    if (__casio_malloc == NULL)
    {
        if (strncmp((void*)0x80020020, "02.01.7002.0000", 15) == 0)
            __casio_malloc = (void*)0x800d0ad8;
        if (__casio_malloc == NULL)
            _bios_panic();
    }
    return ((void*(*)(size_t))__casio_malloc)(size);
}
