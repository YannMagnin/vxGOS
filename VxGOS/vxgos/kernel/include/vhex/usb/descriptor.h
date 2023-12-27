#ifndef __VHEX_USB_DESCRIPTOR_H__
#define __VHEX_USB_DESCRIPTOR_H__ 1

#include <vhex/usb/descriptor/device.h>
#include <vhex/usb/descriptor/configuration.h>
#include <vhex/usb/descriptor/interface.h>
#include <vhex/usb/descriptor/endpoint.h>

/* usb_dc_* - create kernel aliases */
typedef struct usb_descriptor_device        usb_dc_device_t;
typedef struct usb_descriptor_configuration usb_dc_config_t;
typedef struct usb_descriptor_interface     usb_dc_interf_t;
typedef struct usb_descriptor_endpoint      usb_dc_endpnt_t;

#endif /* __VHEX_USB_DESCRIPTOR_H__ */
