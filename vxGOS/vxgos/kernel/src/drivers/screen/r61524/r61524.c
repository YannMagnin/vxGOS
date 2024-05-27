//---
// drivers:screen:r61524:r61524 - low-level driver
//---

#include <stdint.h>
#include <alloca.h>

#include "vhex/drivers/screen/r61524.h"

//---
// Internals
//---

/* Registers and operations */
enum {
    device_code_read            = 0x000,
    driver_output_control       = 0x001,
    driving_wave_control        = 0x002,
    entry_mode                  = 0x003,
    display_control_2           = 0x008,
    low_power_control           = 0x00b,

    ram_address_horizontal      = 0x200,
    ram_address_vertical        = 0x201,
    write_data                  = 0x202,
    read_data                   = 0x202,

    horizontal_ram_start        = 0x210,
    horizontal_ram_end          = 0x211,
    vertical_ram_start          = 0x212,
    vertical_ram_end            = 0x213,
};

/* r61524_select() : select screen mode
 *
 * @note
 * - we reset the RS bit back because the display driver is optimized for
 *   consecutive GRAM access. LCD-transfers will be faster when executing
 *   select() followed by several calls to write() */
static inline void _r61524_hw_select(uint16_t reg)
{
    *(volatile uint8_t *)0xa405013c &= ~0x10;
    __asm__ volatile ("synco"::);
    *(volatile uint16_t *)0xb4000000 = reg;
    __asm__ volatile ("synco"::);
    *(volatile uint8_t *)0xa405013c |= 0x10;
    __asm__ volatile ("synco"::);
}

/* r61524_write() : write information */
static inline void _r61524_hw_write(uint16_t data)
{
    *(volatile uint16_t *)0xb4000000 = data;
}

/* _r61524_hw_read() : read selected register information */
static inline uint16_t _r61524_hw_read(void)
{
    return *(volatile uint16_t *)0xb4000000;
}

/* _r61524_hw_setup_window() : setup windows for reading or writing */
static inline void _r61524_hw_setup_window(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2
) {
    _r61524_hw_select(horizontal_ram_start);
    _r61524_hw_write(395 - x2);
    _r61524_hw_select(horizontal_ram_end);
    _r61524_hw_write(395 - x1);
    _r61524_hw_select(vertical_ram_start);
    _r61524_hw_write(y1);
    _r61524_hw_select(vertical_ram_end);
    _r61524_hw_write(y2);
    _r61524_hw_select(ram_address_horizontal);
    _r61524_hw_write(0);
    _r61524_hw_select(ram_address_vertical);
    _r61524_hw_write(0);
}

//---
// Public
//---

// driver primitivies

/* r61524_hw_configure() : prepare the driver context */
void r61524_hw_configure(struct r61524_drv_ctx *context)
{
    context->HSA = 0;
    context->HEA = 395;
    context->VSA = 0;
    context->VEA = 223;
}

/* r61524_hw_hsave() : save the current driver context */
void r61524_hw_hsave(struct r61524_drv_ctx *context)
{
    _r61524_hw_select(horizontal_ram_start);
    context->HSA = _r61524_hw_read();
    _r61524_hw_select(horizontal_ram_end);
    context->HEA = _r61524_hw_read();
    _r61524_hw_select(vertical_ram_start);
    context->VSA = _r61524_hw_read();
    _r61524_hw_select(vertical_ram_end);
    context->VEA = _r61524_hw_read();
}

/* r61524_hw_hrestore() : restore saved context */
void r61524_hw_hrestore(struct r61524_drv_ctx const *context)
{
    _r61524_hw_select(horizontal_ram_start);
    _r61524_hw_write(context->HSA);
    _r61524_hw_select(horizontal_ram_end);
    _r61524_hw_write(context->HEA);
    _r61524_hw_select(vertical_ram_start);
    _r61524_hw_write(context->VSA);
    _r61524_hw_select(vertical_ram_end);
    _r61524_hw_write(context->VEA);
}


// drawing primitives

void r61524_hw_debug(void)
{
    extern void kterm_clear(int);
    extern void kterm_write(void *,...);

    _r61524_hw_select(device_code_read);
    kterm_write("DEVICE\t\t= %x\n", _r61524_hw_read());
    _r61524_hw_select(driver_output_control);
    kterm_write("DRVOUT\t\t= %x\n", _r61524_hw_read());
    _r61524_hw_select(driving_wave_control);
    kterm_write("WAVE\t\t= %x\n", _r61524_hw_read());
    _r61524_hw_select(entry_mode);
    kterm_write("ENTRY\t\t= %x\n", _r61524_hw_read());

#if 0
    uint16_t *buffer;

    buffer = alloca(396 * sizeof(uint16_t));
    if (buffer == NULL)
        return;
    r61524_hw_dclear(0xf800);
    _r61524_hw_setup_window(10, 11, 395, 224);
    _r61524_hw_select(read_data);
    for (int i = 0 ; i < 385 ; ++i) {
        buffer[i] = _r61524_hw_read();
    }
    //r61524_hw_vram_fetch(0, 0, 395, 1, buffer);

    extern void kterm_clear(int);
    extern void kterm_write(void *,...);

    kterm_clear(0x001f);
    for (int i = 0 ; i < 10 ; ++i) {
        kterm_write("vram[%d] = %p\n", i, buffer[i]);
    }
#endif
    while (1) { __asm__ volatile ("sleep"); }

}

/* r61524_hw_dclear() : optimised screen clearing without vram */
int r61524_hw_dclear(int color)
{
    color = color & 0xffff;
    _r61524_hw_setup_window(0, 0, 395, 223);
    _r61524_hw_select(write_data);
    for (int i = 0; i < 396 * 224; ++i) {
        _r61524_hw_write(color);
    }
    return 0;
}

/* r61524_hw_dpixel() : drawing pixel on screen */
int r61524_hw_dpixel(int x, int y, int color)
{
    if ((unsigned)x >= 396 || (unsigned)y >= 224)
        return -1;
    _r61524_hw_setup_window(x, y, x, y);
    _r61524_hw_select(write_data);
    _r61524_hw_write(color & 0xffff);
    return 0;
}

/* r61524_hw_dscroll() : scroll the screen down to up */
int r61524_hw_dscroll(int offset, int fill_color)
{
    uint16_t *buffer;

    if ((unsigned)offset >= 224)
        return r61524_hw_dclear(fill_color);
    fill_color = fill_color & 0xffff;
    buffer = alloca(396 * sizeof(uint16_t));
    if (buffer == NULL)
        return -1;
    for (int i = 0 ; i < 224 - offset ; ++i)
    {
        r61524_hw_vram_fetch(0, i + offset, 395, i + offset, buffer);
        r61524_hw_vram_send(0, i, 395, i, buffer);
    }
    for (int i = 0 ; i < 396 ; ++i)
        buffer[i] = fill_color;
    for (int i = 224 - offset ; i < 224 ; ++i)
        r61524_hw_vram_send(0, i, 395, i, buffer);
    return 0;
}


// vram


/* r61524_hw_vram_send() : send the vram to the screen
 *
 * @todo
 * - DMA support */
int r61524_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
) {
    int y3;
    int x3;

    _r61524_hw_setup_window(x1, y1, x2, y2);
    _r61524_hw_select(write_data);
    x3 = x2 - x1;
    x3 = x3 + (x3 == 0);
    y3 = y2 - y1;
    y3 = y3 + (y3 == 0);
    for (int i = 0; i < x3 * y3; ++i) {
        _r61524_hw_write(vram[i]);
    }
    return 0;
}

/* r61524_hw_vram_fetch() : dump on-screen pixels
 *
 * @note
 * - the physical driver do not update the internal VRAM counter when
 *   reading, so, we need to perform a manual write (with the read out
 *   pixel) to force counter update which is slow but work */
int r61524_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
) {
    int y3;
    int x3;

    _r61524_hw_setup_window(x1, y1, x2, y2);
    x3 = x2 - x1;
    x3 = x3 + (x3 == 0);
    y3 = y2 - y1;
    y3 = y3 + (y3 == 0);
    int i = 0;
    for (int y = 0 ; y < y3 ; ++y)
    {
        for (int x = 0 ; x < x3 ; ++x)
        {
            _r61524_hw_select(read_data);
            _r61524_hw_read();
            vram[i] = _r61524_hw_read();
            _r61524_hw_select(write_data);
            _r61524_hw_write(vram[i]);
            i += 1;
        }
    }

#if 0
    _r61524_hw_read();
    for (int i = 0 ; i < x3 * y3 ; ++i)
    {
        vram[i] = _r61524_hw_read();
    }
#endif
    return 0;
}
