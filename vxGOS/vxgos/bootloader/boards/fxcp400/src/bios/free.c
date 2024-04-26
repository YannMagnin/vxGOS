#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "bootloader/bios.h"

//---
// Internals
//---

/* __casio_free() : free wrapper */
static void* __casio_free = NULL;

//---
// Public
//---

/* _bios_free() : free wrapper */
void _bios_free(void *ptr)
{
    if (__casio_free == NULL)
    {
        if (strncmp((void*)0x80020020, "02.01.7002.0000", 15) == 0)
            __casio_free = (void*)0x800d0ab0;
        if (__casio_free == NULL)
            _bios_panic();
    }
    ((void*(*)(void*))__casio_free)(ptr);
}
