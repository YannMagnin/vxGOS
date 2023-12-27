#include <vhex/usb/info.h>
#include <vhex/usb.h>

/* internal module information */
extern struct usb_mod_info usb_mod_info;

//---
// Public API
//---

/* usb_open() : open primitive */
int usb_open(struct usb_full_device *device)
{
    if (usb_mod_info.driver.open != NULL)
        return usb_mod_info.driver.open(device);
    return -1;
}
