//---
// drivers:screen:r61523    - Casio's fxcp400 screen
//---

#include <stdint.h>

#include "vhex/drivers/screen/r61523.h"

//---
// Internals
//---

/* Registers and operations */
enum {
    set_column_address  = 0x2a,
    set_page_address    = 0x2b,
    write_memory_start  = 0x2c,
    set_address_mode    = 0x36,
};

/* r61523_select() : select screen mode
 *
 * @note
 * - we reset the RS bit back because the display driver is optimized for
 *   consecutive GRAM access. LCD-transfers will
 *   be faster when executing select() followed by several calls to
 *   write() */
static inline void _r61523_hw_select(uint16_t reg)
{
    *(volatile uint8_t *)0xa405013c &= ~0x10;
    __asm__ volatile ("synco"::);
    *(volatile uint16_t *)0xb4000000 = reg;
    __asm__ volatile ("synco"::);
    *(volatile uint8_t *)0xa405013c |= 0x10;
    __asm__ volatile ("synco"::);
}

/* r61523_write() : write information */
static inline void _r61523_hw_write(uint16_t data)
{
    *(volatile uint16_t *)0xb4000000 = data;
}

//---
// Public
//---

/* r61523_hw_dclear() : optimised screen clearing without vram */
int r61523_hw_dclear(int color)
{
    color = color & 0xffff;
    _r61523_hw_select(set_column_address);
    _r61523_hw_write(40);
    _r61523_hw_write(40);
    _r61523_hw_write(((319 + 40) >> 8) & 0x03);
    _r61523_hw_write(((319 + 40) >> 0) & 0xff);
    _r61523_hw_select(set_page_address);
    _r61523_hw_write(0);
    _r61523_hw_write(0);
    _r61523_hw_write((527 >> 8) & 0x03);
    _r61523_hw_write((527 >> 0) & 0xff);
    _r61523_hw_select(write_memory_start);
    for (int i = 0; i < 320*528; ++i) {
        _r61523_hw_write(color);
    }
    return 0;
}

/* r61523_hw_dpixel() : drawing pixel on screen */
int r61523_hw_dpixel(int x, int y, int color)
{
    int x1;
    int y1;
    int x2;
    int y2;

    if ((unsigned)x >= 320 || (unsigned)y >= 520)
        return -1;
    x1 = x + 40 + 0;
    x2 = x + 40 + 1;
    y1 = y + 0;
    y2 = y + 1;
    _r61523_hw_select(set_column_address);
    _r61523_hw_write((x1 >> 8) & 0x03);
    _r61523_hw_write((x1 >> 0) & 0xff);
    _r61523_hw_write((x2 >> 8) & 0x03);
    _r61523_hw_write((x2 >> 0) & 0xff);
    _r61523_hw_select(set_page_address);
    _r61523_hw_write((y1 >> 8) & 0x03);
    _r61523_hw_write((y1 >> 0) & 0xff);
    _r61523_hw_write((y2 >> 8) & 0x03);
    _r61523_hw_write((y2 >> 0) & 0xff);
    _r61523_hw_select(write_memory_start);
    _r61523_hw_write(color & 0xffff);
    return 0;
}

/* r61523_hw_dscroll() : scroll the screen */
int r61523_hw_dscroll(int offset, int direction)
{
    (void)offset;
    (void)direction;
    return -1;
}

/* r61523_hw_vram_send() : send the vram to the screen
 *
 * @todo
 * - DMA support */
int r61523_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
) {
    x1 = x1 + 40;
    x2 = x2 + 40;
    _r61523_hw_select(set_column_address);
    _r61523_hw_write((x1 >> 8) & 0x03);
    _r61523_hw_write((x1 >> 0) & 0xff);
    _r61523_hw_write((x2 >> 8) & 0x03);
    _r61523_hw_write((x2 >> 0) & 0xff);
    _r61523_hw_select(set_page_address);
    _r61523_hw_write((y1 >> 8) & 0x03);
    _r61523_hw_write((y1 >> 0) & 0xff);
    _r61523_hw_write((y2 >> 8) & 0x03);
    _r61523_hw_write((y2 >> 0) & 0xff);
    _r61523_hw_select(write_memory_start);
    for (unsigned int i = x1 * y1; i < x2 * y2; ++i) {
        _r61523_hw_write(vram[i]);
    }
    return 0;
}
