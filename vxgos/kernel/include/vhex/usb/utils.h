#ifndef __VHEX_USB_UTILS_H__
#define __VHEX_USB_UTILS_H__ 1

/* USB_WORD_TO() - convert word data to USB compatible one */
#define USB_WORD_TO(data) ((((data) & 0xff00) >> 8) | (((data) & 0x00ff) << 8))

/* USB_REQ_DESCT_* - USB GET_DESCRIPTOR request type list */
enum {
    USB_REQ_DESCT_DEVICE    = 1,
    USB_REQ_DESCT_CONFIG    = 2,
    USB_REQ_DESCT_STRING    = 3,
    USB_REQ_DESCT_INTERFACE = 4,
    USB_REQ_DESCT_ENDPOINT  = 5,
    USB_REQ_DESCT_DEVQUAL   = 6,
    USB_REQ_DESCT_OSC       = 7,
    USB_REQ_DESCT_POWER     = 8,
    USB_REQ_DESCT_OTG       = 9,
};

#endif /* __VHEX_USB_UTILS_H__ */
