#ifndef __VHEX_DRIVER_SCREEN_R61524__
# define __VHEX_DRIVER_SCREEN_R61524__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>

/* Registers and operations */
enum {
    device_code_read            = 0x000,
    driver_output_control       = 0x001,
    entry_mode                  = 0x003,
    display_control_2           = 0x008,
    low_power_control           = 0x00b,

    ram_address_horizontal      = 0x200,
    ram_address_vertical        = 0x201,
    write_data                  = 0x202,

    horizontal_ram_start        = 0x210,
    horizontal_ram_end          = 0x211,
    vertical_ram_start          = 0x212,
    vertical_ram_end            = 0x213,
};


VINLINE static void r61524_select(uint16_t reg)
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

VINLINE static uint16_t r61524_read(void)
{
    return *(volatile uint16_t *)0xb4000000;
}

VINLINE static void r61524_write(uint16_t data)
{
    *(volatile uint16_t *)0xb4000000 = data;
}

#endif /* __VHEX_DRIVER_SCREEN_R61524__ */
