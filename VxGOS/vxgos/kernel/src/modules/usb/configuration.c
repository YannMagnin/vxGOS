#include <vhex/usb/configuration.h>
#include <vhex/usb/descriptor.h>
#include <vhex/driver/cpu.h>
#include <vhex/usb/utils.h>
#include <vhex/usb/pipe.h>
#include <vhex/usb.h>

#include <string.h>

//---
// Internals
// @TODO
// - strict descriptor check
// - proper return code error
// - check if the USB connection is already setup
// - configuration selector (SET_CONFIGURATION)
//---

/* internal copy of each descriptor */
static usb_dc_device_t usb_dc_device;
static usb_dc_config_t usb_dc_config[__USB_MAX_DC_CONF];
static usb_dc_interf_t usb_dc_interf[__USB_MAX_DC_INTR];
static usb_dc_endpnt_t usb_dc_endpnt[__USB_MAX_DC_ENDP];

/* internals descriptor counter (used in request interrupt handler) */
static int usb_dc_device_nb = 0;
static int usb_dc_config_nb = 0;
static int usb_dc_interf_nb = 0;
static int usb_dc_endpnt_nb = 0;

/* usb_conf_check_conf() : check and patch interface desc. */
static int usb_conf_check_endpoint(usb_dc_endpnt_t **endpoint, size_t *total)
{
    int pipe_id;
    int i;

    if (endpoint == NULL)
        return -3;
    for (i = 0 ; endpoint[i] != NULL ; ++i)
    {
        if (i >= __USB_MAX_DC_ENDP) {
            usb_logger_write("- too many endpoint (%d)\n", i);
            return -4;
        }
        *total += sizeof(usb_dc_endpnt_t);
        pipe_id = usb_pipe_find(endpoint[i]->bmAttributes.trans);
        if (pipe_id < 0)
            return pipe_id;
        endpoint[i]->bEndpointAddress.number = pipe_id;
        memcpy(
            &usb_dc_endpnt[usb_dc_endpnt_nb],
            endpoint[i],
            sizeof(usb_dc_endpnt_t)
        );
        usb_logger_write("- use pipe %d\n", pipe_id);
        usb_dc_endpnt_nb += 1;
    }
    return i;
}

/* usb_conf_check_conf() : check and patch interface desc. */
static int usb_conf_check_interface(
    struct usb_full_interface **interface,
    size_t *total
) {
    int nbendpoint;
    int i;

    if (interface == NULL)
        return -3;
    for (i = 0 ; interface[i] != NULL ; ++i)
    {
        if (i >= __USB_MAX_DC_INTR) {
            usb_logger_write("- too many interface (%d)\n", i);
            return -4;
        }
        //FIXME : check strings
        *total += sizeof(usb_dc_interf_t);
        nbendpoint = usb_conf_check_endpoint(interface[i]->endpoint, total);
        if (nbendpoint < 0)
            return nbendpoint;
        interface[i]->descriptor->bInterfaceNumber = i;
        interface[i]->descriptor->bNumEndpoints = nbendpoint;
        memcpy(
            &usb_dc_interf[usb_dc_interf_nb],
            interface[i]->descriptor,
            sizeof(usb_dc_interf_t)
        );
        usb_logger_write("- use interface %d\n", usb_dc_interf_nb);
        usb_dc_interf_nb += 1;
    }
    return i;
}

/* usb_conf_check_conf() : check and patch configuration desc. */
static int usb_conf_check_conf(struct usb_full_config **config)
{
    int nbinterf;
    size_t total;
    int i;

    if (config == NULL)
        return -2;
    for (i = 0 ; config[i] != NULL ; ++i)
    {
        if (i >= __USB_MAX_DC_CONF) {
            usb_logger_write("- too many configuration (%d)\n", i);
            return -4;
        }
        //FIXME : check strings
        total = sizeof(struct usb_descriptor_configuration);
        nbinterf = usb_conf_check_interface(config[i]->interface, &total);
        if (nbinterf < 0)
            return nbinterf;
        config[i]->descriptor->wTotalLengh = USB_WORD_TO(total);
        config[i]->descriptor->bConfigurationValue = i;
        config[i]->descriptor->bNumInterfaces = nbinterf;
        memcpy(
            &usb_dc_config[usb_dc_config_nb],
            config[i]->descriptor,
            sizeof(usb_dc_config_t)
        );
        usb_logger_write(
            "- use configuration %d with %d interface\n",
            usb_dc_config_nb,
            nbinterf
        );
        usb_dc_config_nb += 1;
    }
    return i;
}

//---
// Public API
//---

/* usb_config_resolve() : check and resolve USB configuration */
int usb_config_resolve(struct usb_full_device *device)
{
    int nbconf;

    if (device == NULL)
        return -1;

    //FIXME : check if the USB connection is already set
    //FIXME : check string validity

    cpu_atomic_start();

    /* USB descriptor counter reset */
    usb_dc_device_nb = 1;
    usb_dc_config_nb = 0;
    usb_dc_interf_nb = 0;
    usb_dc_endpnt_nb = 0;


    /* patch configuration descriptors */
    nbconf = usb_conf_check_conf(device->config);
    if (nbconf < 0)
        return nbconf;

    /* patch device descriptor */
    device->descriptor->bNumConfigurations = nbconf;
    memcpy(&usb_dc_device, device->descriptor, sizeof(usb_dc_device_t));

    cpu_atomic_end();
    return 0;
}

/* usb_config_select_config() : try to selecte a configuration */
int usb_config_select_config(int idx)
{
    int ret;

    if (idx < 0)
        return -1;

    cpu_atomic_start();
    ret = (idx < usb_dc_config_nb) ? 0 : -2;
    //TODO : real configuration selection
    cpu_atomic_end();
    return ret;
}

/* usb_config_get_dc_device() : get device descriptor */
int usb_config_get_dc_device(usb_dc_device_t **dc_device, int idx)
{
    if (dc_device == NULL)
        return -1;
    if (idx != 0) {
        *dc_device = NULL;
        return -2;
    }
    *dc_device = &usb_dc_device;
    return 0;
}

/* usb_config_get_dc_config() : get config descriptor */
int usb_config_get_dc_config(
    usb_dc_config_t **dc_config,
    int idx,
    int *isidx,
    int *ieidx
) {
    int ret;
    int start;
    int end;

    if (dc_config == NULL)
        return -1;
    if (idx < 0)
        return -2;

    cpu_atomic_start();
    ret = -3;
    *dc_config = NULL;
    if (idx < usb_dc_config_nb) {
        start = 0;
        for (int i = 0 ; i < idx ; ++i) {
            start += usb_dc_config[i].bNumInterfaces;
        }
        end = start + usb_dc_config[idx].bNumInterfaces;
        *dc_config = &usb_dc_config[idx];
        if (isidx != NULL) *isidx = start;
        if (ieidx != NULL) *ieidx = end;
        ret = 0;
    }
    cpu_atomic_end();

    return ret;
}

/* usb_config_get_dc_interf() : get interface descriptor */
int usb_config_get_dc_interf(
    usb_dc_interf_t **dc_interf,
    int idx,
    int *esidx,
    int *eeidx
) {
    int ret;
    int start;
    int end;

    if (dc_interf == NULL)
        return -1;
    if (idx < 0)
        return -2;

    cpu_atomic_start();
    ret = -3;
    *dc_interf = NULL;
    if (idx < usb_dc_interf_nb) {
        start = 0;
        for (int i = 0 ; i < idx ; ++i) {
            start += usb_dc_interf[i].bNumEndpoints;
        }
        end = start + usb_dc_interf[idx].bNumEndpoints;
        *dc_interf = &usb_dc_interf[idx];
        if (esidx != NULL) *esidx = start;
        if (eeidx != NULL) *eeidx = end;
        ret = 0;
    }
    cpu_atomic_end();

    return ret;
}

/* usb_config_get_dc_endpoint() : get endpoint descriptor */
int usb_config_get_dc_endpnt(usb_dc_endpnt_t **dc_endpnt, int idx)
{
    int ret;

    if (dc_endpnt == NULL)
        return -1;

    cpu_atomic_start();
    ret = -2;
    *dc_endpnt = NULL;
    if (idx < usb_dc_endpnt_nb) {
        *dc_endpnt = &usb_dc_endpnt[idx];
        ret = 0;
    }
    cpu_atomic_end();

    return ret;
}
