//---
// drivers:screen:t6k11     - Casio's fxcp400 screen
//---

#include <stdint.h>
#include <alloca.h>

#include "vhex/drivers/screen/t6k11.h"
#include "vhex/_klibc.h"

//---
// Internal
//---

/* indicate which real screen primitives will be used between t6k13 or
 * ML9801 */
static void *__t6k11_hw_dclear      = &ml9801_hw_dclear;
static void *__t6k11_hw_dpixel      = &ml9801_hw_dpixel;
static void *__t6k11_hw_vram_send   = &ml9801_hw_vram_send;
static void *__t6k11_hw_vram_fetch  = &ml9801_hw_vram_fetch;

//---
// Public
//---

// driver

/* expose VRAM here */
uint32_t t6k11_vram[((128 * 64) / 8) / 4];

/* t6k11_hw_configure() : configure the driver (detect real screen)
 *
 * @notes
 * - Tell if it is a Graph 35+E II using the OS version (this is accurate
 *   unless someone tweaks an OS file enough to de-correlate the version
 *   of the OS and the version of the display and storage memory drivers,
 *   which, let's be real, is enough for now. */
void t6k11_hw_configure(struct t6k11_drv_ctx *_ctx)
{
    if (((char *)0x80010020)[1] == '3')
        return;
    __t6k11_hw_dclear       = (void*)&t6k73_hw_dclear;
    __t6k11_hw_dpixel       = (void*)&t6k73_hw_dpixel;
    __t6k11_hw_vram_send    = (void*)&t6k73_hw_vram_send;
    __t6k11_hw_vram_fetch   = (void*)&t6k73_hw_vram_fetch;
    (void)_ctx;
}

// drawing

/* t6k11_hw_dclear() : optimised screen clearing without vram */
int t6k11_hw_dclear(int color)
{
    return ((int(*)(int))__t6k11_hw_dclear)(color);
}

/* t6k11_hw_dpixel() : drawing pixel on screen */
int t6k11_hw_dpixel(int x, int y, int color)
{
    return ((int(*)(int,int,int))__t6k11_hw_dpixel)(x, y, color);
}

/* t6k11_hw_dscroll() : scroll the screen down to up */
int t6k11_hw_dscroll(int offset, int fill_color)
{
    uint16_t *buffer;

    if ((unsigned)offset >= T6K11_SCREEN_HEIGHT)
        return t6k11_hw_dclear(fill_color);
    buffer = alloca((T6K11_SCREEN_WIDTH * T6K11_SCREEN_WIDTH) / 8);
    if (buffer == NULL)
        return -1;
    klibc_memset(
        buffer,
        (fill_color == 0x0000) ? 0x00 : 0xff,
        (T6K11_SCREEN_WIDTH * T6K11_SCREEN_WIDTH) / 8
    );
    t6k11_hw_vram_fetch(
        0,
        offset,
        T6K11_SCREEN_WIDTH,
        T6K11_SCREEN_HEIGHT,
        buffer
    );
    t6k11_hw_vram_send(
        0,
        0,
        T6K11_SCREEN_WIDTH,
        T6K11_SCREEN_WIDTH,
        buffer
    );
    return 0;
}

// vram

/* t6k11_hw_vram_send() : send the vram to the screen */
int t6k11_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
) {
    return (
        (int(*)(
            unsigned int,
            unsigned int,
            unsigned int,
            unsigned int,
            void *
        ))
        __t6k11_hw_vram_send
    )(x1, y1, x2, y2, vram);
}

/* t6k11_hw_vram_fetch() : dump on-screen pixel */
int t6k11_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
) {
    return (
        (int(*)(
            unsigned int,
            unsigned int,
            unsigned int,
            unsigned int,
            void *
        ))
        __t6k11_hw_vram_fetch
    )(x1, y1, x2, y2, vram);
}
