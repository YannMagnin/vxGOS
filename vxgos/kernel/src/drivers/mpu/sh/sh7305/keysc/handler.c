#include <vhex/keyboard.h>
#include <vhex/driver/mpu/sh/sh7305/keysc.h>
#include <vhex/driver/mpu/sh/sh7305/intc.h>


extern void sh7305_keycache_update(vkey_t key, int status);

/* sh7305_keysc_int_handler() : SH7305 KEYSC interrupt handler */
void sh7305_keysc_int_handler(void)
{
    int intlevel;
    int column;
    vkey_t key;
    int row;

    /* Block / disable KEYSC interrupt (otherwise the module will crash) */
    SH7305_INTC._MSK->IMR5.KEYSC = 1;
    intlevel = SH7305_INTC._IPRX->IPRF.KEYSC;
    SH7305_INTC._IPRX->IPRF.KEYSC = 0;

    /* update the keycache */
    row = 6;
    key = 0x5f;
    while (--row >= 0) {
        column = 16;
        while (--column >= 0) {
            sh7305_keycache_update(
                key,
                SH7305_KEYSC.KIUDATA[row] & (0x8000 >> column)
            );
            key -= 1;
        }
    }

    /* Clear KEYSC interrupt, really */
    SH7305_KEYSC.KIUINTREG.word = SH7305_KEYSC.KIUINTREG.word;

    /* Unblock / enable KEYSC interrupt. */
    SH7305_INTC._IPRX->IPRF.KEYSC = intlevel;
    SH7305_INTC._MSKCLR->IMR5.KEYSC = 1;
}
