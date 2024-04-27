#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


void kernel_debug(void)
{
    volatile uint16_t *pacr     = (void*)0xa4050100;
    volatile uint16_t *psela    = (void*)0xa405014e;
    volatile uint8_t  *ddk_cncr = (void*)0xa44c0020;
    volatile uint16_t *ddclk0   = (void*)0xa44c0000;

    /* LED on */
    *pacr  = *pacr  & 0x03fc;       // configure clock to special output
    *psela = *psela & 0xfcff;       // Stop module.
    *psela = *psela | 0x0100;       // configure LED pin to ouput
    *ddk_cncr = *ddk_cncr & 0xfe;   // Stop External Clock 0
    *ddclk0 = 0xc001;               // LED to low color
    *ddk_cncr = *ddk_cncr | 0x01;   // Start the clock

    /* wait user-reset */
    while (true) {
        __asm__ volatile ("nop");
    }
}
