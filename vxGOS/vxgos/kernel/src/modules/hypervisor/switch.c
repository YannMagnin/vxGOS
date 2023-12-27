#include <vhex/hypervisor.h>
#include <vhex/driver/cpu.h>
#include <vhex/display.h>

/* hypervisor_world_switch() : perform a world switch */
//TODO: handle shared drivers
int hypervisor_world_switch(hyp_world_t out, hyp_world_t in)
{
    struct vhex_world *win;
    struct vhex_world *wout;
    struct vhex_driver *dtable;

    wout = hypervisor_world_get(out);
    win = hypervisor_world_get(in);
    if (wout == NULL || win == NULL)
        return (hyp_world_not_found);
    if (in == out)
        return (0);

    cpu_atomic_start();
    dtable = vhex_driver_table();
    for (int i = 0; i < vhex_driver_count(); ++i) {
        if (dtable[i].hpowered != NULL && dtable[i].hpowered() == false) {
            wout->driver[i].flags.UNUSED = 1;
            continue;
        }
        if (dtable[i].hsave == NULL) {
            wout->driver[i].flags.UNUSED = 1;
            continue;
        }
        dtable[i].hsave(wout->driver[i].context);
        wout->driver[i].flags.UNUSED = 0;
    }
    for (int i = 0; i < vhex_driver_count(); ++i) {
        if (win->status.INIT == 0) {
            win->driver[i].flags.UNUSED = dtable[i].flags.UNUSED;
            win->driver[i].flags.SHARED = dtable[i].flags.SHARED;
            if (win->driver[i].flags.UNUSED == 0)
                dtable[i].configure(win->driver[i].context);
        }
        //FIXME : check shared driver
        if (win->driver[i].flags.UNUSED == 1) {
            if (dtable[i].hpowered != NULL && dtable[i].hpowered() == true)
                dtable[i].hpoweroff();
            continue;
        }
        //FIXME : check if the driver is unused
        if (dtable[i].hpowered != NULL && dtable[i].hpowered() == false)
            dtable[i].hpoweron();
        if (dtable[i].hrestore != NULL)
            dtable[i].hrestore(win->driver[i].context);
    }
    win->status.INIT = 1;
    win->status.ACTIVE = 1;
    wout->status.ACTIVE = 0;
    wout->status.INIT = 1;
    cpu_atomic_end();
    return 0;
}
