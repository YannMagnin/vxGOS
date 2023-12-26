#ifndef VXGOS_BOOTLOADER_RASPI3B_MEMORY_H
#define VXGOS_BOOTLOADER_RASPI3B_MEMORY_H

#include <stdint.h>
#include <stddef.h>

//---
// Public API
//---

enum {
    MEM_FLAG_DISCARDABLE        = 1 << 0,
    MEM_FLAG_NORMAL             = 0 << 2,
    MEM_FLAG_DIRECT             = 1 << 2,
    MEM_FLAG_COHERENT           = 2 << 2,
    MEM_FLAG_L1_NONALLOCATING   = (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT),
    MEM_FLAG_ZERO               = 1 << 4,
    MEM_FLAG_NO_INIT            = 1 << 5,
    MEM_FLAG_HINT_PERMALOCK     = 1 << 6,
};

/* memory_alloc() : malloc using GPU firmware request */
extern void *memory_alloc(size_t size, int align, int flags);

/* memory_free() : free using GPU firmware request */
extern void memory_free(void *ptr);

#endif /* VXGOS_BOOTLOADER_RASPI3B_MEMORY_H */
