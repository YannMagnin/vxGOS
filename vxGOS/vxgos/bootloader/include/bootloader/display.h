#ifndef BOOTLOADER_DISPLAY_H
#define BOOTLOADER_DISPLAY_H 1

#include <stdint.h>
#include <stddef.h>

/* color alias */
#define C_WHITE 0xffff
#define C_BLACK 0x0000

//---
// Types
//---

/* struct font: Font data encoded for bootloader dfont*() functions */
struct font
{
    uint32_t *data;
    uint32_t count;
    uint8_t  height;
    uint8_t  width;
    uint8_t  line_height;
    uint16_t char_block_size;
};

//---
// Public API
//---

/* dinfo() : get display information */
extern void dinfo(size_t *width, size_t *height);

/* dfont_get() : get font information */
extern int dfont_get(struct font **font);

/* dclear() : clear VRAM with unified color */
extern void dclear(int color);

/*dtext() : display raw text to VRAM */
extern void dtext(int x, int y, int color, char const *text);

/* dprint() : printf-like dtext() */
extern void dprint(int x, int y, int color, char const *format, ...);

/* dascii() : draw char using provided font */
extern void dascii(int x, int y, int color, char n, struct font *font);

/* dline() : display line */
extern void dline(int x0, int y0, int x1, int y1, int color);

/* dpixel() : draw pixel */
extern void dpixel(int x, int y, int color);

/* dupdate() : update VRAM to screen */
extern void dupdate(void);

#endif /* BOOTLOADER_DISPLAY_H */
