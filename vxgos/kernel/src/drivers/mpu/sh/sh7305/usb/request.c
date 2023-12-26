#include <vhex/driver/mpu/sh/sh7305/usb.h>
#include <vhex/usb/configuration.h>
#include <vhex/usb/utils.h>
#include <vhex/display.h>
#include <vhex/usb.h>

#include <stdarg.h>

//---
// Internal error API
//---

/* sh7305_usb_request_unimplemented() : not implemented request */
static void sh7305_usb_request_unimplemented(
    char const * const name,
    int wValue,
    int wIndex,
    int wLength
) {
    dclear(C_WHITE);
    dprint(0, 0, C_BLACK, "%s (%d, %d, %d)", name, wValue, wIndex, wLength);
    dupdate();
    while (1) { __asm__ volatile ("sleep"); }
}

/* sh7305_usb_request_error() : USB request error */
static void sh7305_usb_request_error(char const * const text, ...)
{
    va_list ap;

    va_start(ap, text);

    dclear(C_WHITE);
    dvprint(0, 0, C_BLACK, text, ap);
    dupdate();
    while (1) { __asm__ volatile ("sleep"); }
}

/* sh7305_usb_request_get_descriptor() : send descriptor through DCP */
static void sh7305_usb_request_get_descriptor(
    int wValue,
    int wIndex,
    size_t wLength
) {
    usb_dc_device_t *dc_device;
    usb_dc_config_t *dc_config;
    usb_dc_interf_t *dc_interf;
    usb_dc_endpnt_t *dc_endpnt;
    int isidx;
    int ieidx;
    int esidx;
    int eeidx;
    int type;
    int indx;

    (void)wIndex;
    (void)wLength;

    /* fetch descriptor request */
    type = (wValue & 0xff00) >> 8;
    indx = (wValue & 0x00ff) >> 0;

    /* Return the DEVICE descriptor
     * @notes
     * - This is the first request, we only need to return the device
     *   descriptor. */
    if (type == USB_REQ_DESCT_DEVICE) {
        usb_logger_write("DCP write DEVICE_DESC (%04X)\n", wValue);
        if (usb_config_get_dc_device(&dc_device, indx) != 0) {
            sh7305_usb_request_error("unable to find the device descriptor\n");
            //FIXME : handle error
        }
        sh7305_usb_dcp_prepare();
        sh7305_usb_dcp_write(dc_device, sizeof(usb_dc_device_t));
        sh7305_usb_dcp_commit();
        return;
    }

    /* Return the CONFIGURATION descriptor
     * @notes
     * - this request is performed 2 times : on the first request, we should
     *   return only the descriptor. On the second request, wLengh is set to
     *   the .wTotalLengh and we should return the current configuration
     *   descriptor and all of its interfaces / endpoints descriptors */
    if (type == USB_REQ_DESCT_CONFIG)
    {
        usb_logger_write("DCP write CONFIG_DESC (%d,%d)\n", indx, wLength);
        if (usb_config_get_dc_config(&dc_config, indx, &isidx, &ieidx) != 0) {
            sh7305_usb_request_error("- config %d not found\n", indx);
            //FIXME : handle error
        }
        sh7305_usb_dcp_prepare();
        sh7305_usb_dcp_write(dc_config, sizeof(usb_dc_config_t));
        if (wLength <= sizeof(usb_dc_config_t)) {
            sh7305_usb_dcp_commit();
            return;
        }
        for (int i = isidx ; i < ieidx ; i++) {
            if (usb_config_get_dc_interf(&dc_interf, i, &esidx, &eeidx) != 0) {
                sh7305_usb_request_error("- interface %d not found\n", i);
                //FIXME : handle error
            }
            sh7305_usb_dcp_write(dc_interf, sizeof(usb_dc_interf_t));
            for (int j = esidx ; j < eeidx ; ++j) {
                if (usb_config_get_dc_endpnt(&dc_endpnt, j) != 0) {
                    sh7305_usb_request_error("- endpoint %d not found\n", j);
                    //FIXME : handle error
                }
                sh7305_usb_dcp_write(dc_endpnt, sizeof(usb_dc_endpnt_t));
            }
        }
        sh7305_usb_dcp_commit();
        return;
    }

    /* TODO : support all other request */
    if (type == USB_REQ_DESCT_STRING) {
        sh7305_usb_request_unimplemented("GET_DESC_STR", indx, 0, 0);
        return;
    }
    if (type == USB_REQ_DESCT_INTERFACE) {
        sh7305_usb_request_unimplemented("GET_DESC_INTERFACE", indx, 0, 0);
        return;
    }
    if (type == USB_REQ_DESCT_ENDPOINT) {
        sh7305_usb_request_unimplemented("GET_DESC_ENDPOINT", indx, 0, 0);
        return;
    }
    if (type == USB_REQ_DESCT_DEVQUAL) {
        sh7305_usb_request_unimplemented("GET_DESC_DEVQUAL", indx, 0, 0);
        return;
    }
    if (type == USB_REQ_DESCT_OSC) {
        sh7305_usb_request_unimplemented("GET_DESC_OSC", indx, 0, 0);
        return;
    }
    if (type == USB_REQ_DESCT_POWER) {
        sh7305_usb_request_unimplemented("GET_DESC_POWER", indx, 0, 0);
        return;
    }
    if (type == USB_REQ_DESCT_OTG) {
        sh7305_usb_request_unimplemented("GET_DESC_OTG", indx, 0, 0);
        return;
    }

    /* handle USB error */
    sh7305_usb_request_error(
        "GET_DESCRIPTOR : unable to resolve type %d (%04X)",
        type, wValue
    );
}


//---
// Public API
//---

/* sh7305_usb_request_handle() : handle USB request */
void sh7305_usb_request_handle(void)
{
    int bmRequestType;
    int bRequest;
    int wValue;
    int wIndex;
    int wLength;

    /* fetch request information */
    bmRequestType = SH7305_USB.USBREQ.BMREQUEST;
    bRequest      = SH7305_USB.USBREQ.BREQUEST;
    wValue        = SH7305_USB.USBVAL.word;
    wIndex        = SH7305_USB.USBINDX.word;
    wLength       = SH7305_USB.USBLENG.word;

    usb_logger_write("wLength = %d\n", SH7305_USB.USBLENG.word);

    /* clear VALID bits */
    SH7305_USB_INTSTS0_CLEAR(VALID);

    /* check GET request */
    if (bmRequestType == 0x80) {
        if (bRequest == SH7305_USB_REQT_GET_STATUS) {
            sh7305_usb_request_unimplemented("GET_STATUS", wValue, wIndex, wLength);
            return;
        }
        if (bRequest == SH7305_USB_REQT_GET_DESCRIPTOR) {
            sh7305_usb_request_get_descriptor(wValue, wIndex, wLength);
            return;
        }
        if (bRequest == SH7305_USB_REQT_GET_CONFIGURATION) {
            sh7305_usb_request_unimplemented(
                "GET_CONFIGURATION", wValue, wIndex, wLength
            );
            return;
        }
        usb_logger_write("request: unable to resolve bRequest %d\n", bRequest);
        return;
    }

    /* check SET request */
    if (bmRequestType == 0x00) {
        if (bRequest == SH7305_USB_REQT_CLEAR_FEATURE) {
            sh7305_usb_request_unimplemented(
                "CLEAR_FEATURE", wValue, wIndex, wLength
            );
            return;
        }
        if (bRequest == SH7305_USB_REQT_SET_FEATURE) {
            sh7305_usb_request_unimplemented(
                "SET_FEATURE", wValue, wIndex, wLength
            );
            return;
        }
        if (bRequest == SH7305_USB_REQT_SET_ADDRESS) {
            sh7305_usb_request_unimplemented(
                "SET_ADDRESS", wValue, wIndex, wLength
            );
            return;
        }
        if (bRequest == SH7305_USB_REQT_SET_DESCRIPTOR) {
            sh7305_usb_request_unimplemented(
                "SET_DESCRIPTOR", wValue, wIndex, wLength
            );
            return;
        }
        if (bRequest == SH7305_USB_REQT_SET_CONFIGURATION) {
            usb_logger_write("SET_CONFIGURATION<%d>", wIndex);
            sh7305_usb_dcp_prepare();
            usb_config_select_config(wIndex);
            //FIXME : proper DCP FIFO config and handle config selection error
            sh7305_usb_dcp_commit();
            return;
        }
        usb_logger_write("request: unable to resolve bRequest %d\n", bRequest);
        return;
    }

    /* non-standar error */
    sh7305_usb_request_error(
        "request: unable to resolve bnRequestType %d\n", bmRequestType
    );
}
