#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "bootloader/bios.h"

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


/* r61523_select() : select screen mode */
static inline void r61523_select(uint16_t reg)
{
    /* Clear RS and write the register number */
    *(volatile uint8_t *)0xa405013c &= ~0x10;
    __asm__ volatile ("synco"::);
    *(volatile uint16_t *)0xb4000000 = reg;
    __asm__ volatile ("synco"::);

    /* Set RS back. We don't do this in read()/write() because the display
       driver is optimized for consecutive GRAM access. LCD-transfers will
       be faster when executing select() followed by several calls to
       write() */
    *(volatile uint8_t *)0xa405013c |= 0x10;
    __asm__ volatile ("synco"::);
}

/* r61523_write() : write information */
static inline void r61523_write(uint16_t data)
{
    *(volatile uint16_t *)0xb4000000 = data;
}

//---
// Public
//---

/* use the VRAM used by Casio */
// (fixme) : check portability of this address
uint16_t volatile *casio_vram = (void*)0x8c000000;

/* _bios_dupdate() : small R61523 driver
 *
 * @note
 * Casio uses an undocumented register at 0xDA, which seems to return
 * the screen variant currently used (which does not concern us here). In
 * the case that this is our screen which is detected, Casio add an extra 40
 * pixels to the starting width and ending width.
 *
 * This arbitrary padding appears to match the R61523 datasheet, which
 * explains that the original screen has a size of 360x640 pixels, and
 * adding 40 pixels to our original starting area yields 360 pixels.
 *
 * (todo) : better understanding of the special 0xda register
 * */
void _bios_dupdate(void)
{
    /* Set the windows size */
    r61523_select(set_column_address);
    r61523_write(40);
    r61523_write(40);
    r61523_write(((319 + 40) >> 8) & 0x03);
    r61523_write(((319 + 40) >> 0) & 0xff);
    r61523_select(set_page_address);
    r61523_write(0);
    r61523_write(0);
    r61523_write((527 >> 8) & 0x03);
    r61523_write((527 >> 0) & 0xff);

    /* Bind address 0xb4000000 to the data write command and send vram */
    r61523_select(write_memory_start);
    for (int i = 0; i < 320*528; ++i) {
        r61523_write(casio_vram[i]);
    }
}
