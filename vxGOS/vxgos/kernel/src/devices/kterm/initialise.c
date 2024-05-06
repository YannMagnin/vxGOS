//---
// device:kterm:initialise  - initialise the kernel terminal
//---

#include <string.h>

#include "vhex/devices/kterm.h"
#include "vhex/modules/display/interface.h"
#include "vhex/drivers/driver.h"

//---
// Public
//---

/*_kterm : internal kernel terminal object */
struct _kterm _kterm;

/* kterm_init() : initialise the kernel terminal
 *
 * Note that we are early in kernel booting process. So, neither drivers or
 * modules are initialised which signify that we will manually initialise the
 * "display" driver.
 *
 * The display driver will provide (in its internal data) critical
 * information about the hardware like the screen size and screen primitives
 * that we will directly use through the device since we cannot use the
 * drawing stack (the display module)
 *
 * We assume that we are in an hardware state that allow us to perform a
 * power-on and configuration without saving anything */
int kterm_init(void)
{
    struct vxdriver *driver;

    memset(&_kterm, 0x00, sizeof(struct _kterm));
    if (kernel_driver_find(&driver, "display") != 0)
        return -1;
    memcpy(
        &_kterm.display.driver,
        (struct dstack_drv_interface*)driver->module_data,
        sizeof(struct dstack_drv_interface)
    );
    if (_kterm.display.driver.bios.dclear == NULL)
        return -2;
    if (_kterm.display.driver.bios.dpixel == NULL)
        return -3;
    if (_kterm.display.driver.bios.dscroll == NULL)
        return -4;
    if (driver->hpowered != NULL && driver->hpowered() == false) {
        driver->hpoweron();
        driver->configure(NULL);
    }
    _kterm.__magic = _KTERM_INIT_MAGIC;
    kterm_clear(0x00000000);
    return 0;
}
