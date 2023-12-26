#ifndef __VHEX_USB_INTERFACE_H__
#define __VHEX_USB_INTERFACE_H__ 1

#include <vhex/usb/configuration.h>

struct usb_drv_interface
{
    /* "classic" primitives API */
    int (*open)(struct usb_full_device *device);

    /* pipe API */
    int (*pipe_find)(int type);
};

#endif /* __VHEX_USB_INTERFACE_H__ */
