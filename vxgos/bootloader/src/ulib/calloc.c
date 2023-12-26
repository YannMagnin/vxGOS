#include <stdlib.h>
#include <string.h>

//---
// Public
//---

/* calloc() : malloc + memset */
void *calloc(size_t nmemb, size_t size)
{
    unsigned int total_size;
    void *mem;

    if(__builtin_umul_overflow(nmemb, size, &total_size))
        return NULL;

    mem = malloc(total_size);
    if(mem != NULL)
        memset(mem, 0, total_size);
    return mem;
}
