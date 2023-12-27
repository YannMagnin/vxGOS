#ifndef VXGOS_BOOTLOADER_RASPI3B_UTILS_H
#define VXGOS_BOOTLOADER_RASPI3B_UTILS_H

#include <stdint.h>
#include <stddef.h>

//---
// macros helpers
//---

/* Giving a type to padding bytes is misguiding, let's hide it in a macro */
#define pad_nam2(c) _ ## c
#define pad_name(c) pad_nam2(c)
#define pad(bytes)  uint8_t pad_name(__COUNTER__)[bytes]

/* Packed structures.
 * I require explicit alignment because if it's unspecified, GCC cannot
 * optimize access size, and reads to memory-mapped I/O with invalid access
 * sizes silently fail - honestly you don't want this to happen */
#define VPACKED(x)  __attribute__((packed, aligned(x)))

/* uint32_t union helper */
#define u32_union(name, fields)         \
    union {                             \
        uint32_t lword;                 \
        struct { fields } VPACKED(4);   \
    } VPACKED(4) name

/* UTIL_SPINWAIT_CYCLES() : wait at least X cycles */
//TODO : handle superscalar behaviour
//TODO : handle over/under clock
#define UTIL_SPINWAIT_CYCLES(x)         \
    for(int r = x ; r > 0 ; r--) {      \
        __asm__ volatile ("nop");       \
    }

#endif /* VXGOS_BOOTLOADER_RASPI3B_UTILS_H */
