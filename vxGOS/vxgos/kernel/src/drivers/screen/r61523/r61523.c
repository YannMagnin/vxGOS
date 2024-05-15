//---
// drivers:screen:r61523    - Casio's fxcp400 screen
//---

#include <stdint.h>
#include <alloca.h>

#include "vhex/drivers/screen/r61523.h"

//---
// Internals
//---

/* Registers and operations */
enum {
    get_power_mode          = 0x0a,
    get_address_mode        = 0x0b,
    get_pixel_format        = 0x0c,
    get_display_mode        = 0x0d,
    get_signal_mode         = 0x0e,
    get_diagnostic_result   = 0x0f,
    set_column_address      = 0x2a,
    set_page_address        = 0x2b,
    write_memory_start      = 0x2c,
    read_memory_start       = 0x2e,
    set_address_mode        = 0x36,
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

/* _r61523_hw_read() : read selected register information */
static inline uint16_t _r61523_hw_read(void)
{
    return *(volatile uint16_t *)0xb4000000;
}

/* _r61523_hw_setup_window() : setup windows for reading or writing */
static inline void _r61523_hw_setup_window(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2
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
}

//---
// Public
//---

// driver

void r61523_hw_debug(void)
{
    extern void kterm_write(void *a, ...);

    _r61523_hw_select(get_power_mode);
    kterm_write("POWER_MODE\t%x\n", _r61523_hw_read());
    _r61523_hw_select(get_address_mode);
    kterm_write("ADDR_MODE\t%x\n", _r61523_hw_read());
    _r61523_hw_select(get_pixel_format);
    kterm_write("PIXEL_MODE\t%x\n", _r61523_hw_read());
    _r61523_hw_select(get_display_mode);
    kterm_write("DISP_MODE\t%x\n", _r61523_hw_read());
    _r61523_hw_select(get_signal_mode);
    kterm_write("SIG_MODE\t%x\n", _r61523_hw_read());
    _r61523_hw_select(get_diagnostic_result);
    kterm_write("DIAG_MODE\t%x\n", _r61523_hw_read());
}

// drawing

/* r61523_hw_dclear() : optimised screen clearing without vram */
int r61523_hw_dclear(int color)
{
    color = color & 0xffff;
    _r61523_hw_setup_window(0, 0, 319, 527);
    _r61523_hw_select(write_memory_start);
    for (int i = 0; i < 320*528; ++i) {
        _r61523_hw_write(color);
    }
    return 0;
}

/* r61523_hw_dpixel() : drawing pixel on screen */
int r61523_hw_dpixel(int x, int y, int color)
{
    if ((unsigned)x >= 320 || (unsigned)y >= 528)
        return -1;
    _r61523_hw_setup_window(x + 0, y + 0, x + 1, y + 1);
    _r61523_hw_select(write_memory_start);
    _r61523_hw_write(color & 0xffff);
    return 0;
}

/* r61523_hw_dscroll() : scroll the screen down to up */
int r61523_hw_dscroll(int offset, int fill_color)
{
    uint16_t *buffer;

    if ((unsigned)offset >= 528)
        return r61523_hw_dclear(fill_color);
    fill_color = fill_color & 0xffff;
    buffer = alloca(320 * sizeof(uint16_t));
    if (buffer == NULL)
        return -1;
    for (int i = 0 ; i < 528 - offset ; ++i)
    {
        r61523_hw_vram_fetch(0, i + offset, 319, i + offset, buffer);
        r61523_hw_vram_send(0, i, 319, i, buffer);
    }
    for (int i = 0 ; i < 320 ; ++i)
        buffer[i] = fill_color;
    for (int i = 528 - offset ; i < 528 ; ++i)
        r61523_hw_vram_send(0, i, 319, i, buffer);
    return 0;
}

// vram

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
    int y3;
    int x3;

    _r61523_hw_setup_window(x1, y1, x2, y2);
    _r61523_hw_select(write_memory_start);
    x3 = x2 - x1;
    x3 = x3 + (x3 == 0);
    y3 = y2 - y1;
    y3 = y3 + (y3 == 0);
    for (int i = 0; i < x3 * y3; ++i) {
        _r61523_hw_write(vram[i]);
    }
    return 0;
}

/* r61523_hw_vram_fetch() : dump on-screen pixels
 *
 * @note
 * - pixel format is always 24-bit. So, we need at least two read per pixel
 * - the first read is useless and seems used internally to avoid to ensure
 *   that the second read will always start with the red and green color
 *   information (the user do not have to handle how to decode color
 *   for "unaligned" pixel like:
 *
 *      0xRRGG 0xBBRR 0xGGBB 0xRRGG ...
 *      '---------''-------'   '--- ...
 *           0         1
 *  for example the first pixel will have an encoding RRGGBBxx and the second
 *  xxRRGGBB which is not the same handling. I suspect the dummy read to
 *  re-align the decoding for us) */
int r61523_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
) {
    uint16_t frag0;
    uint16_t frag1;
    int state;
    int counter;
    int y3;
    int x3;
    int r;
    int g;
    int b;

    _r61523_hw_setup_window(x1, y1, x2, y2);
    _r61523_hw_select(read_memory_start);
    x3 = x2 - x1;
    x3 = x3 + (x3 == 0);
    y3 = y2 - y1;
    y3 = y3 + (y3 == 0);
    state = 0;
    counter = 0;
    _r61523_hw_read();
    while (counter < x3 * y3)
    {
        switch (state)
        {
            case 0:
                frag0 = _r61523_hw_read();
                state = 1;
                break;
            case 1:
                frag1 = _r61523_hw_read();
                r = ((((frag0 & 0xff00) >> 8) * 32) / 256) & 0x1f;
                g = ((((frag0 & 0x00ff) >> 0) * 64) / 256) & 0x3f;
                b = ((((frag1 & 0xff00) >> 8) * 32) / 256) & 0x1f;
                vram[counter] = (r << 11) | (g << 5) | (b << 0);
                counter += 1;
                state = 2;
                break;
            case 2:
                frag0 = _r61523_hw_read();
                r = ((((frag1 & 0x00ff) >> 0) * 32) / 256) & 0x1f;
                g = ((((frag0 & 0xff00) >> 8) * 64) / 256) & 0x3f;
                b = ((((frag0 & 0x00ff) >> 0) * 32) / 256) & 0x1f;
                vram[counter] = (r << 11) | (g << 5) | (b << 0);
                counter += 1;
                state = 0;
                break;
            default:
                break;
        }
    }
    return 0;
}
