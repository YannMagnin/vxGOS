#ifndef BOOTLOADER_BIOS_H
#define BOOTLOADER_BIOS_H 1

#include <stdint.h>
#include <stddef.h>

// opaque definition of font struct
struct font;

/* _bios_dinfo() : get display information */
extern void _bios_dinfo(size_t *width, size_t *height);

/* _bios_dfont_get() : get font information */
extern int _bios_dfont_get(struct font **font);

/* _bios_dpixel() : draw pixel */
extern void _bios_dpixel(int x, int y, int color);

/* _bios_dudpate() : display VRAM to screen */
extern void _bios_dupdate(void);

/* _bios_free() : free allocated memory */
extern void _bios_free(void *ptr);

/* _bios_malloc() : memory allocator */
extern void *_bios_malloc(size_t size);

/* _bios_panic() : bios panic (fatal error) */
extern _Noreturn void _bios_panic(void);

#endif /* BOOTLOADER_BIOS_H */
