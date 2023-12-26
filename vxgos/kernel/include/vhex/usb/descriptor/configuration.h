#ifndef __VHEX_USB_DESCRIPTOR_CONFIGURATION_H__
#define __VHEX_USB_DESCRIPTOR_CONFIGURATION_H__ 1

#include <vhex/defs/types.h>

/* usb_descriptor_configuration - USB Configuration Descriptor struct */
struct usb_descriptor_configuration {
    uint8_t  bLengh;                /* size of this descriptor in bytes */
    uint8_t  bDescriptorType;       /* USB_DESCRIPTOR_CONFIG */
    uint16_t wTotalLengh;           /* total lengh of configuration data */
    uint8_t  bNumInterfaces;        /* number of interface */
    uint8_t  bConfigurationValue;   /* configuration index (start at 1!) */
    uint8_t  iConfiguration;        /* index of string desc. */

    /* Configuration characteristics
     * @notes
     * - power  1=self-powered
     * - wakeup 1=remote wakeup support */
    byte_union_alone(bmAttributes,
        uint8_t         : 1;
        uint8_t power   : 1;
        uint8_t wakeup  : 1;
        uint8_t         : 4;
    );

    /* Maximum power that USB should provide
     * @notes
     * - only provided if bmAttributes.power=1
     * - exposed as bMaxPower=1 -> 2mA (ex: bMaxPower=50 -> 100mA) */
    uint8_t bMaxPower;
} VPACKED_ALONE;

//---
// Helpers
//---

#include <vhex/usb/descriptor/types.h>

/* USB_DECLARE_DESC_CONF - helper to declare configuration descriptor */
#define USB_DECLARE_DESC_CONF(name, ...)                                \
    struct usb_descriptor_configuration name =                          \
        USB_DECLARE_DESC_CONF_RAW(__VA_ARGS__)

/* USB_DECLARE_DESC_CONF_RAW - helper to declare default content */
#define USB_DECLARE_DESC_CONF_RAW(...)                                  \
    {                                                                   \
        .bLengh          = sizeof(struct usb_descriptor_configuration), \
        .bDescriptorType = USB_DESCRIPTOR_CONFIGURATION,                \
        __VA_ARGS__                                                     \
    }

#endif /* __VHEX_USB_DESCRIPTOR_CONFIGURATION_H__ */
