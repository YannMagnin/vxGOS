#include <vhex/usb/info.h>
#include <vhex/usb.h>

/* internal module information */
extern struct usb_mod_info usb_mod_info;

//---
// Public API
//---

/* usb_pipe_find() : find a pipe of type `type` */
int usb_pipe_find(int type)
{
    if (usb_mod_info.driver.pipe_find != NULL)
        return usb_mod_info.driver.pipe_find(type);
    return -1;
}
