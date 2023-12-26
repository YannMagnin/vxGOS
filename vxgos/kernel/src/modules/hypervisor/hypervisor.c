#include "vhex/hypervisor.h"
#include "vhex/module.h"

/* module init */

/* __hypervisor_init() : initialize the hypervisor */
static void __hypervisor_init(void)
{
    /* NOTE: this part SHOULD be provided by the board definition */
    extern void hypervisor_init(void);
    hypervisor_init();
}

/* __hypervisor_quit() : uninit the hypervisor */
static void __hypervisor_quit(void)
{
    /* NOTE: this part SHOULD be provided by the board definition */
    extern void hypervisor_quit(void);
    hypervisor_quit();
}

/* declare the hypervisor module */

struct vhex_module mod_hypervisor = {
    .name = "hypervisor",
    .init = &__hypervisor_init,
    .quit = &__hypervisor_quit,
};
VHEX_DECLARE_MODULE(02, mod_hypervisor);
