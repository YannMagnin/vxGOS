#if 0
#include <stdbool.h>

#include "bootloader/display.h"
#include "bootloader/bios.h"

#include "bootloader/console.h"

void debug_main(void)
{
    uint16_t volatile *SH7305_KEYSC = (void*)0xa44b0000;
    int keycode;

    keycode = 0x0000;
    while (true) {
        dclear(C_BLACK);
        dprint(0, 0, C_WHITE, "keycode = %p", keycode);
        for (int i = 0; i < 6 ; ++i) {
            dprint(
                0, (14*(i + 1)), C_WHITE,
                "KEYSCDATA[%d] = %p",
                i, SH7305_KEYSC[i]
            );
        }
        dupdate();
        keycode = keysc_fetch();
    }
}
#endif
