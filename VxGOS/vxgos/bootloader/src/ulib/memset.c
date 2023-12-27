#include <stdint.h>
#include <stddef.h>

//---
// Public
//---

/* memset() : wipe arena */
void *memset(void *s, int c, size_t n)
{
    while ((int)--n >= 0)
        ((uint8_t *) s)[n] = c;
    return s;
}
