#ifndef VXGOS_BOOTLOADER_RASPI3B_FRAMEBUFFER_H
#define VXGOS_BOOTLOADER_RASPI3B_FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

//---
// Public API
//---

#define FRAMEBUFFER_WATERMARK   0xdeb0cad0

/* frambuffer - internal framebuffer information */
struct framebuffer
{
    uint32_t watermark;
    struct {
        uintptr_t gpu;
        uintptr_t cpu;
        size_t size;
        int pitch;
        int align;
    } vram;
    struct {
        size_t width;
        size_t height;
    } phys;
    struct {
        size_t width;
        size_t height;
    } virt;
};

/* framebuffer_init() : initialize framebuffer */
extern int framebuffer_init(void);

/* framebuffer_get() : fetch the framebuffer information if available */
extern int framebuffer_get(struct framebuffer **framebuffer_out);

/* framebuffer_debug() : display framebuffer information */
extern void framebuffer_debug(void);

#endif /* VXGOS_BOOTLOADER_RASPI3B_FRAMEBUFFER_H */
