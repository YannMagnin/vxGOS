//---
// drivers:screen:t6k73    - Casio's "old" monochrom screen
//---

#include "vhex/drivers/screen/t6k11.h"

//---
// Internals
//---

/* register information
 *
 * @notes
 * - this screen do not have public documentation. Each register has been
 *   found using reverse engineering */
enum {
    reg_counter = 1,
    reg_yaddr   = 4,
    reg_xaddr   = 4,
    reg_data    = 7,
};

/* _t6k73_select() : select screen mode
 *
 * @note
 * - we reset the RS bit back because the display driver is optimized for
 *   consecutive GRAM access. LCD-transfers will
 *   be faster when executing select() followed by several calls to
 *   write() */
static inline void _t6k73_hw_select(uint8_t reg)
{
    *(volatile uint8_t *)0xb4000000 = reg;
}

/* _t6k73_write() : write information */
static inline void _t6k73_hw_write(uint8_t data)
{
    *(volatile uint8_t *)0xb4010000 = data;
}

/* _t6k73_hw_read() : read selected register information */
static inline uint8_t _t6k73_hw_read(void)
{
    return *(volatile uint8_t *)0xb4010000;
}

//---
// Public
//---

/* t6k73_hw_dclear() : clear screen with color */
int t6k73_hw_dclear(int color)
{
    uint8_t bitmap;

    bitmap = (color == 0x0000) ? 0x00 : 0xff;
    for(int y = 0; y < 64; y++)
    {
        _t6k73_hw_select(reg_yaddr);
        _t6k73_hw_write(y | 0xc0);
        _t6k73_hw_select(reg_xaddr);
        _t6k73_hw_write(0);
        _t6k73_hw_select(reg_data);
        for (int x = 0 ; x < 128 / 8 ; ++x)
            _t6k73_hw_write(bitmap);
    }
    return 0;
}

/* t6k73_hw_dpixel() : drawing pixel on screen */
int t6k73_hw_dpixel(int x, int y, int color)
{
    uint8_t bitmap;

    _t6k73_hw_select(reg_yaddr);
    _t6k73_hw_write(y | 0xc0);
    _t6k73_hw_select(reg_xaddr);
    _t6k73_hw_write(x / 8);
    _t6k73_hw_select(reg_data);
    bitmap = _t6k73_hw_read();
    _t6k73_hw_select(reg_yaddr);
    _t6k73_hw_write(y | 0xc0);
    _t6k73_hw_select(reg_xaddr);
    _t6k73_hw_write(x / 8);
    _t6k73_hw_select(reg_data);
    if (color != 0x0000) {
        _t6k73_hw_write(bitmap | (0x80 >> (x & 7)));
    } else {
        _t6k73_hw_write(bitmap & ~(0x80 >> (x & 7)));
    }
    return 0;
}

/* t6k73_hw_vram_send() : send the vram to the screen */
int t6k73_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint8_t *vram
){
    for (unsigned int y = y1 ; y < y2 ; y++)
    {
        _t6k73_hw_select(reg_yaddr);
        _t6k73_hw_write(y | 0xc0);
        _t6k73_hw_select(reg_xaddr);
        _t6k73_hw_write(x1 / 8);
        _t6k73_hw_select(reg_data);
        for (unsigned int x = (x1 / 8) ; x < (x2 / 8) ; x++)
        {
            _t6k73_hw_write(vram[0]);
            vram = &(vram[1]);
        }
    }
    return 0;
}

/* t6k73_hw_vram_fetch() : fetch "on-screen" pixels */
int t6k73_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint8_t *vram
){
    for (unsigned int y = y1 ; y < y2 ; y++)
    {
        _t6k73_hw_select(reg_yaddr);
        _t6k73_hw_write(y | 0xc0);
        _t6k73_hw_select(reg_xaddr);
        _t6k73_hw_write(x1 / 8);
        _t6k73_hw_select(reg_data);
        for (unsigned int x = (x1 / 8) ; x < (x2 / 8) ; x++)
        {
            vram[0] = _t6k73_hw_read();
            vram = &(vram[1]);
        }
    }
    return 0;
}
