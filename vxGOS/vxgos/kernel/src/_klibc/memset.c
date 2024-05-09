//---
// _klibc:memset    - kernel internal memset
//---

#include "vhex/_klibc.h"

//---
// Public
//---

/* klibc_memset() : wipe arena */
void *klibc_memset(void *s, int c, size_t n)
{
    while ((int)--n >= 0)
        ((uint8_t *)s)[n] = c;
    return s;
}
