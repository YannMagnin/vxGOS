#include <stdint.h>

#include "bootloader/command.h"
#include "bootloader/console.h"

//---
// Internals
//---

static int _command_intab(void)
{
    uintptr_t intevt_table;
    uint32_t r5_bank;
    int intevt;

    __asm__("stc r5_bank,%0" : "=r"(r5_bank));
    intevt_table = ((uintptr_t*)r5_bank)[0];

    //intevt = 0x400;
    //intevt = 0x6e0;
    //intevt = 0x8c0;
    //intevt = 0xba0;
    intevt = 0xe60;
    console_write("R5_BANK - %p\n", r5_bank);
    console_write("INTEVT_TABLE - %p\n", intevt_table);
    for (int i = 0; i < 30 ; ++i)
    {
        console_write(
            "[0x%x] - %p\n",
            intevt,
            ((uintptr_t*)(intevt_table + (intevt >> 2)))[0]
        );
        //console_write(
        //    "[0x%x] - %p\n",
        //    intevt,
        //    ((uintptr_t*)(intevt_table + (intevt >> 2)))[1]
        //);
        intevt += 0x20;
    }
    return 0;
}

//---
// Public
//---

VCMD_DECLARE(
    cmd_systab,
    .name   = "i",
    .desc   = "dump interrupt table",
    .func   = (void*)&_command_intab,
);
