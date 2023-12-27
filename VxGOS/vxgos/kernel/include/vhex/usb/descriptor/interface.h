#ifndef __VHEX_USB_DESCRIPTOR_INTERFACE_H__
#define __VHEX_USB_DESCRIPTOR_INTERFACE_H__ 1

#include <vhex/defs/types.h>

/* usb_descriptor_interface - USB Interface Descriptor struct */
struct usb_descriptor_interface {
    uint8_t bLengh;             /* size of this descriptor in bytes */
    uint8_t bDescriptorType;    /* descriptor type (USB_DESCRIPTOR_DEVICE) */
    uint8_t bInterfaceNumber;   /* the number of this interface */
    uint8_t bAlternateSetting;  /* used to select an alternate settings */
    uint8_t bNumEndpoints;      /* number of endpoint (except endpoint 0) */
    uint8_t bInterfaceClass;    /* class code */
    uint8_t bInterfaceSubClass; /* subclass code */
    uint8_t bInterfaceProtocol; /* protocol code */
    uint8_t iInterface;         /* index of string desc. for this interface */
} VPACKED_ALONE;

//---
// Helpers
//---

#include <vhex/usb/descriptor/types.h>

/* USB_DECLARE_DESC_INTERFACE - helper to declare interface descriptor */
#define USB_DECLARE_DESC_INTERFACE(name, ...)                       \
    struct usb_descriptor_interface name =                          \
        USB_DECLARE_DESC_INTERFACE_RAW(__VA_ARGS__)

/* USB_DECLARE_DESC_INTERFACE_RAW - helper to declare interface content */
#define USB_DECLARE_DESC_INTERFACE_RAW(...)                         \
    {                                                               \
        .bLengh          = sizeof(struct usb_descriptor_interface), \
        .bDescriptorType = USB_DESCRIPTOR_INTERFACE,                \
        __VA_ARGS__                                                 \
    }

#endif /* __VHEX_USB_DESCRIPTOR_DEVICE_H__ */
