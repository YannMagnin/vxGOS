#ifndef __VHEX_USB_DESCRIPTOR_ENDPOINT_H__
#define __VHEX_USB_DESCRIPTOR_ENDPOINT_H__ 1

#include <vhex/defs/types.h>

/* usb_descriptor_endpoint - USB Endpoint Descriptor struct */
struct usb_descriptor_endpoint {
    uint8_t  bLengh;            /* size of this descriptor in bytes */
    uint8_t  bDescriptorType;   /* descriptor type (USB_DESCRIPTOR_DEVICE) */

    /* bEndpointAddress - Endpoint address configuration
     * @notes
     * - dir        0=OUT, 1=IN
     * - number     the endpoint number */
    byte_union_alone(bEndpointAddress,
        uint8_t dir     : 1;
        uint8_t         : 3;
        uint8_t number  : 4;
    );

    /* bmAttributes - Endpoint configuration
     * @notes
     * - usage      usage type (Data, Feedback, Implicit feedback)
     * - sync       synchronization type (No sync., Async., Adaptive, Sync.)
     * - trans      transfert type (Control, Isochronous, bulk, Interrupt) */
    byte_union_alone(bmAttributes,
        uint8_t         : 2;
        uint8_t usage   : 2;
        uint8_t sync    : 2;
        uint8_t trans   : 2;
    );

    /* wPacketSize - Maximum packet size for this endpoint
     * @notes
     * - size       maximum packet size
     * - additional additional transaction opportunities per uframe */
    word_union_alone(wMaxPacketSize,
        uint16_t size       :10;
        uint16_t additional : 2;
        uint16_t            : 4;
    );

    uint8_t bInterval;          /* Interval for polling endpoint */
} VPACKED_ALONE;

//---
// Helpers
//---

#include <vhex/usb/descriptor/types.h>

/* USB_DECLARE_DESC_ENDPOINT - helper to declare endpoint descriptor */
#define USB_DECLARE_DESC_ENDPOINT(name, ...)                        \
    struct usb_descriptor_endpoint name =                           \
        USB_DECLARE_DESC_ENDPOINT_RAW(__VA_ARGS__)

/* USB_DECLARE_DESC_ENDPOINT_RAW - helper to declare endpoint content */
#define USB_DECLARE_DESC_ENDPOINT_RAW(...)                          \
    {                                                               \
        .bLengh          = sizeof(struct usb_descriptor_endpoint),  \
        .bDescriptorType = USB_DESCRIPTOR_ENDPOINT,                 \
        __VA_ARGS__                                                 \
    }

#endif /* __VHEX_USB_DESCRIPTOR_ENDPOINT_H__ */
