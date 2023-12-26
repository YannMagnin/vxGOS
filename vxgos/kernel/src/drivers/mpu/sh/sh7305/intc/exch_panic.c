#include <vhex/display.h>
#include <vhex/defs/types.h>

/* defined by the linker script */
extern uint32_t __kernel_reloc_start;

void sh7305_intc_exch_panic(void)
{
    uint32_t PC;
    uint32_t TEA;
    uint32_t TRA;
    uint32_t code;

    /* get register information */
    __asm__("stc spc, %0" : "=r"(PC));

    TEA  = *(volatile uint32_t *)0xff00000c;
    TRA  = *(volatile uint32_t *)0xff000020 >> 2;
    code = *(volatile uint32_t *)0xff000024;

    dclear(C_WHITE);
    dtext(6, 3, C_BLACK, "An exception occured! (System ERROR)");

//  uint32_t *long_vram = (void *)vhex_vram;
//  for(int i = 0; i < 198 * 16; i++) long_vram[i] = ~long_vram[i];

    char const *name = "";
    if(code == 0x040) name = "TLB miss (nonexisting address) on read";
    if(code == 0x060) name = "TLB miss (nonexisting address) on write";
    if(code == 0x0e0) name = "Read address error (probably alignment)";
    if(code == 0x100) name = "Write address error (probably alignment)";
    if(code == 0x160) name = "Unconditional trap";
    if(code == 0x180) name = "Illegal instruction";
    if(code == 0x1a0) name = "Illegal delay slot instruction";

    dprint(6, 25, C_BLACK, "%03x %s", code, name);

    dtext(  6, 45, C_BLACK, "PC");
    dprint(38, 45, C_BLACK, "= %08x", PC - (uintptr_t)&__kernel_reloc_start);
    dtext(261, 45, C_BLACK, "(Error location)");

    dtext(  6, 60, C_BLACK, "TEA");
    dprint(38, 60, C_BLACK, "= %08x", TEA);
    dtext(234, 60, C_BLACK, "(Offending address)");

    dtext(  6, 75, C_BLACK, "TRA");
    dprint(38, 75, C_BLACK, "= %#x", TRA);
    dtext(281, 75, C_BLACK, "(Trap number)");

    dprint( 6, 90, C_BLACK, "reloc base = %p", &__kernel_reloc_start);

    dtext(6, 120, C_BLACK, "An unrecoverable error occurred in the add-in.");
    dtext(6, 135, C_BLACK, "Please press the RESET button to restart the");
    dtext(6, 150, C_BLACK, "calculator.");
    dupdate();
    while (1) { __asm__ volatile ("sleep"); }
}
