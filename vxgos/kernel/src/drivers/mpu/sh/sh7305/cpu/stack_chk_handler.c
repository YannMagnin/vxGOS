#include <vhex/defs/attributes.h>
#include <vhex/driver/mpu/sh/sh7305/cpu.h>
#include <vhex/display.h>


/* defined by the linker script */
extern uint32_t __kernel_reloc_start;

/* sh7305_cpu_stack_chk_fail() : stack smashing error */
VNO_STACK_PROTECTOR
void sh7305_cpu_stack_chk_fail(struct sh7305_cpu_ctx *ctx)
{
    dclear(C_WHITE);
    dtext(6, 3, C_BLACK, "Stack smashing detected (System ERROR)");

    dtext(  6, 45, C_BLACK, "PC");
    dprint(38, 45, C_BLACK, "= %08x", ctx->pr - (uintptr_t)&__kernel_reloc_start);
    dtext(261, 45, C_BLACK, "(Error location)");

    dprint( 6, 90, C_BLACK, "reloc base = %p", &__kernel_reloc_start);

    dtext(6, 120, C_BLACK, "An unrecoverable error occurred in the add-in.");
    dtext(6, 135, C_BLACK, "Please press the RESET button to restart the");
    dtext(6, 150, C_BLACK, "calculator.");
    dupdate();
    while (1) { __asm__ volatile ("sleep"); }
}
