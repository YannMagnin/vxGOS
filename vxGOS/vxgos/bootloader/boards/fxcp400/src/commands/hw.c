#include <stdint.h>

#include "bootloader/command.h"
#include "bootloader/console.h"

//---
// Internals
//---

static int _command_hw(void)
{
    uintptr_t sr;
    uintptr_t vbr;
    uint32_t volatile *mstpcr0;
    uint32_t volatile *mstpcr1;
    uint32_t volatile *mstpcr2;
    uint32_t volatile *stbcr;
    uint32_t volatile *expmask;
    uint32_t volatile *cpuopm;
    uint32_t r4_bank;
    uint32_t r5_bank;
    uint32_t r6_bank;
    uint32_t r7_bank;

    __asm__("stc vbr,%0" : "=r"(vbr));
    __asm__("stc sr,%0"  : "=r"(sr));
    __asm__("stc r4_bank,%0" : "=r"(r4_bank));
    __asm__("stc r5_bank,%0" : "=r"(r5_bank));
    __asm__("stc r6_bank,%0" : "=r"(r6_bank));
    __asm__("stc r7_bank,%0" : "=r"(r7_bank));
    stbcr   = (void*)0xa4150020;
    mstpcr0 = (void*)0xa4150030;
    mstpcr1 = (void*)0xa4150034;
    mstpcr2 = (void*)0xa4150038;
    expmask = (void*)0xff2f0004;
    cpuopm  = (void*)0xff2f0000;

    console_write("SR\t\t%p\n",    sr);
    console_write("VBR\t\t%p\n",   vbr);
    console_write("EXPMASK\t%p\n", *expmask);
    console_write("CPUOPM\t%p\n",  *cpuopm);
    console_write("STPCR\t%p\n",   *stbcr);
    console_write("MSTPCR0\t%p\n", *mstpcr0);
    console_write("MSTPCR1\t%p\n", *mstpcr1);
    console_write("MSTPCR2\t%p\n", *mstpcr2);
    console_write("R4_BANK\t%p\n", r4_bank);
    console_write("R5_BANK\t%p\n", r5_bank);
    console_write("R6_BANK\t%p\n", r6_bank);
    console_write("R7_BANK\t%p\n", r7_bank);
    return 0;
}

//---
// Public
//---

VCMD_DECLARE(
    cmd_hw,
    .name   = "h",
    .desc   = "display registers",
    .func   = (void*)&_command_hw,
);
