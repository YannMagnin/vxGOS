#ifndef __VHEX_USB_INFO_H__
#define __VHEX_USB_INFO_H__ 1

#include <vhex/usb/interface.h>
#include <vhex/usb/logger.h>

/* internal USB module information */
struct usb_mod_info
{
    struct usb_drv_interface driver;
    struct {
        bool attached;
        usb_logger_t device;
    } logger;
};

/* internaly used by the module */
extern struct usb_mod_info usb_mod_info;

#endif /* __VHEX_USB_INFO_H__ */
