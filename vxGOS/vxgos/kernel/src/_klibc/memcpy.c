//---
// _klibc:memcpy    - kernel internal memcpy
//---

#include "vhex/_klibc.h"

//---
// Public
//---

/* klibc_memcpy() : copy memory arena */
void *klibc_memcpy(void *dest, const void *src, size_t count)
{
    for (size_t i = 0; i < count; i = i + 1)
        ((uint8_t *) dest)[i] = ((uint8_t *) src)[i];
    return dest;
}
