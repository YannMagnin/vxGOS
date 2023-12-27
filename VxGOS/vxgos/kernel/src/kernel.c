#include <vhex/kernel.h>
#include <vhex/module.h>

//---
//  Initialization and unloading
//---

/* kinit(): Install and start vhex */
void kinit(void)
{
    struct vhex_module *mtable;

    mtable = vhex_module_table();
    for (int i = 0; i < vhex_module_count(); ++i) {
        if (mtable[i].init != NULL)
            mtable[i].init();
    }
}

/* kquit(): Quit vhex and give back control to the system */
void kquit(void)
{
    struct vhex_module *mtable;

    mtable = vhex_module_table();
    for (int i = 0; i < vhex_module_count(); ++i) {
        if (mtable[i].quit != NULL)
            mtable[i].quit();
    }
}
