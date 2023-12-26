#ifndef __VHEX_USB_DESCRIPTOR_DEVICE_H__
#define __VHEX_USB_DESCRIPTOR_DEVICE_H__ 1

#include <vhex/defs/types.h>

/* usb_descriptor_device - USB Device Descriptor struct */
struct usb_descriptor_device {
    uint8_t  bLengh;            /* size of this descriptor in bytes */
    uint8_t  bDescriptorType;   /* descriptor type (USB_DESCRIPTOR_DEVICE) */
    uint16_t bcdUSB;            /* USB release number in bin. (0x210 -> 2.1) */
    uint8_t  bDeviceClass;      /* class code */
    uint8_t  bDeviceSubClass;   /* subclass code */
    uint8_t  bDeviceProtocol;   /* protocol code */
    uint8_t  bMaxPacketSize0;   /* packet size of endpoint 0 */
    uint16_t idVendor;          /* vendor ID  (assigned by USB-IF) */
    uint16_t idProduct;         /* product ID (assigned by manufacturer) */
    uint16_t bcdDevice;         /* device release version (0x220 -> 2.20) */
    uint8_t  iManufacturer;     /* index of string descr. for Manufacturer */
    uint8_t  iProduct;          /* index of string descr. for product */
    uint8_t  iSerialNumber;     /* index of string descr. for serial number */
    uint8_t  bNumConfigurations;/* number of possible configuration */
} VPACKED_ALONE;

//---
// helpers
//---

#include <vhex/usb/descriptor/types.h>

/* USB_DECLARE_DESC_DEV - helper to declare device descriptor */
#define USB_DECLARE_DESC_DEVICE(name, ...)                          \
    struct usb_descriptor_device name =                             \
        USB_DECLARE_DESC_DEVICE_RAW(__VA_ARGS__)

/* USB_DECLARE_DESC_DEVICE_RAW - helper to declare default content */
#define USB_DECLARE_DESC_DEVICE_RAW(...)                            \
    {                                                               \
        .bLengh          = sizeof(struct usb_descriptor_device),    \
        .bDescriptorType = USB_DESCRIPTOR_DEVICE,                   \
        __VA_ARGS__                                                 \
    }

#endif /* __VHEX_USB_DESCRIPTOR_DEVICE_H__ */
