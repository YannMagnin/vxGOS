#include <vhex/driver.h>
#include <vhex/module.h>
#include <vhex/usb/interface.h>
#include <vhex/usb/info.h>
#include <vhex/usb.h>

#include <string.h>

//---
// Kernel module information
//---

/* internal USB driver information */
struct usb_mod_info usb_mod_info;

/* __usb_init() : initialize the usb module */
static void __usb_init(void)
{
    struct vhex_driver *driver;

    memset(&usb_mod_info, 0x00, sizeof(usb_mod_info));

    driver = vhex_driver_table();
    for (int i = 0; i < vhex_driver_count(); ++i) {
        if (driver[i].flags.USB) {
            memcpy(
                &usb_mod_info.driver,
                driver[i].module_data,
                sizeof(struct usb_drv_interface)
            );
            break;
        }
    }
}

/* __usb_quit() : uninitialize the usb module */
static void __usb_quit(void)
{
    memset(&usb_mod_info, 0x00, sizeof(usb_mod_info));
}

/* declare the timer module */

struct vhex_module mod_usb = {
    .name = "USB",
    .init = &__usb_init,
    .quit = &__usb_quit,

};
VHEX_DECLARE_MODULE(06, mod_usb);
