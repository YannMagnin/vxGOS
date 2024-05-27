//---
// drivers:screen:ml9801    - Casio's "new" monochrom screen
//---

#include "vhex/drivers/screen/t6k11.h"

//---
// Internals
//---

/* register information
 *
 * @notes
 * - this screen do not have public documentation. Each register has been
 *   found using reverse-ingineering */
enum {
    reg_xaddr   = 8,
    reg_yaddr   = 8,
    reg_data    = 10,
};

/* _ml9801_select() : select screen mode
 *
 * @note
 * - we reset the RS bit back because the display driver is optimized for
 *   consecutive GRAM access. LCD-transfers will
 *   be faster when executing select() followed by several calls to
 *   write() */
static inline void _ml9801_hw_select(uint8_t reg)
{
    *(volatile uint8_t *)0xb4000000 = reg;
}

/* _ml9801_write() : write information */
static inline void _ml9801_hw_write(uint8_t data)
{
    *(volatile uint8_t *)0xb4010000 = data;
}

/* _ml9801_hw_read() : read selected register information */
static inline uint8_t _ml9801_hw_read(void)
{
    return *(volatile uint8_t *)0xb4010000;
}

//---
// Public
//---

/* ml9801_hw_dclear() : clear screen with color */
int ml9801_hw_dclear(int color)
{
    uint8_t bitmap;

    bitmap = (color == 0x0000) ? 0x00 : 0xff;
    for(int y = 0; y < 64; y++)
    {
        _ml9801_hw_select(reg_yaddr);
        _ml9801_hw_write(y | 0x80);
        _ml9801_hw_select(reg_xaddr);
        _ml9801_hw_write(4);
        _ml9801_hw_select(reg_data);
        for (int x = 0 ; x < 128 / 8 ; ++x)
            _ml9801_hw_write(bitmap);
    }
    return 0;
}

/* ml9801_hw_dpixel() : drawing pixel on screen */
int ml9801_hw_dpixel(int x, int y, int color)
{
    uint8_t bitmap;

    _ml9801_hw_select(reg_yaddr);
    _ml9801_hw_write(y | 0x80);
    _ml9801_hw_select(reg_xaddr);
    _ml9801_hw_write(4 + (x / 8));
    _ml9801_hw_select(reg_data);
    bitmap = _ml9801_hw_read();
    _ml9801_hw_select(reg_yaddr);
    _ml9801_hw_write(y | 0x80);
    _ml9801_hw_select(reg_xaddr);
    _ml9801_hw_write(4 + (x / 8));
    _ml9801_hw_select(reg_data);
    if (color != 0x0000) {
        _ml9801_hw_write(bitmap | (0x80 >> (x & 7)));
    } else {
        _ml9801_hw_write(bitmap & ~(0x80 >> (x & 7)));
    }
    return 0;
}

/* ml9801_hw_vram_send() : send the vram to the screen */
int ml9801_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
){
    (void)x1;
    (void)x2;
    (void)y1;
    (void)y2;
    (void)vram;
    return -1;
}

/* ml9801_hw_vram_fetch() : fetch "on-screen" pixels */
int ml9801_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
){
    (void)x1;
    (void)x2;
    (void)y1;
    (void)y2;
    (void)vram;
    return -1;
}
