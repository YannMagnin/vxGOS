#ifndef __VHEX_USB_CONFIGURATION_H__
#define __VHEX_USB_CONFIGURATION_H__ 1

#include <vhex/usb/descriptor.h>

/* usb_full_interface - interface descriptor, names, and endpoints */
struct usb_full_interface {
    usb_dc_interf_t *descriptor;
    char **strs;
    usb_dc_endpnt_t **endpoint;
};

/* usb_full_config - interface descriptor, names, and interfaces */
struct usb_full_config {
    usb_dc_config_t *descriptor;
    char **strs;
    struct usb_full_interface **interface;
};

/* usb_full_device - interface descriptor, names, and configurations */
struct usb_full_device {
    usb_dc_device_t *descriptor;
    char **strs;
    struct usb_full_config **config;
};

/* __USB_MAX_* - internal configuration limit */
#define __USB_MAX_DC_CONF    2
#define __USB_MAX_DC_INTR    4
#define __USB_MAX_DC_ENDP    9

/* usb_config_resolve() : check and resolve USB configuration */
extern int usb_config_resolve(struct usb_full_device *device);

/* usb_config_select_config() : try to selecte a configuration */
extern int usb_config_select_config(int idx);

/* usb_config_get_dc_device() : get device descriptor */
extern int usb_config_get_dc_device(usb_dc_device_t **dc_device, int idx);

/* usb_config_get_dc_config() : get config descriptor */
extern int usb_config_get_dc_config(
    usb_dc_config_t **dc_config,
    int idx,
    int *isidx,
    int *ieidx
);

/* usb_config_get_dc_interf() : get interface descriptor */
extern int usb_config_get_dc_interf(
    usb_dc_interf_t **dc_interf,
    int idx,
    int *esidx,
    int *eeidx
);

/* usb_config_get_dc_endpoint() : get endpoint descriptor */
extern int usb_config_get_dc_endpnt(usb_dc_endpnt_t **dc_endpoint, int idx);

#endif /* __VHEX_USB_CONFIGURATION_H__ */
