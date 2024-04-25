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


/* r61523_select() : selecte screen mode */
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
       write(). (Although most applications should use the DMA instead.) */
    *(volatile uint8_t *)0xa405013c |= 0x10;
    __asm__ volatile ("synco"::);
}

/* r61523_read() : read information from screen */
static inline uint16_t r61523_read(void)
{
    return *(volatile uint16_t *)0xb4000000;
}

/* r61523_write() : write information */
static void r61523_write(uint16_t data)
{
    *(volatile uint16_t *)0xb4000000 = data;
}

//---
// Public
//---

/* _bios_dupdate() : small R61523 driver
 *
 * @note
 * Casio use an undocumented register at 0xda which seems return either the
 * screen variant that is currently used (which is our case here, this is
 * clearly not a `r61523` device). In the case that this is our screen that
 * is detected, an extra 40 pixels should be added to the starting width and
 * ending width.
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

    /* Bind address 0xb4000000 to the data write command */
    r61523_select(write_memory_start);

    /* send VRAM */
    for (int i = 0; i < 320*528; ++i) {
        r61523_write(0x001f);
    }
}
