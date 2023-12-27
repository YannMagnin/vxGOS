#include <vhex/usb/info.h>
#include <vhex/usb.h>
#include <vhex/driver/cpu.h>

#include <string.h>

/* internal module information */
extern struct usb_mod_info usb_mod_info;

//---
// Public API
//---

/* usb_logger_set() : return true if a logger is attached */
bool usb_logger_is_attached(void)
{
    return usb_mod_info.logger.attached;
}

/* usb_logger_set() : attach a logger to the kernel USB driver (clear) */
int usb_logger_attach(usb_logger_t *logger, size_t buffer_size)
{
    int ret;

    if (logger == NULL)
        return -1;

    cpu_atomic_start();
    ret  = -2;
    if (usb_mod_info.logger.attached == false) {
        ret = -3;
        if (logger->primitive.init != NULL)
            ret = logger->primitive.init(logger, buffer_size);
    }
    if (ret == 0) {
        memcpy(&usb_mod_info.logger.device, logger, sizeof(usb_logger_t));
        usb_mod_info.logger.attached = true;
    }
    cpu_atomic_end();

    return ret;
}

/* usb_logger_clear() : clear the logger buffer */
int usb_logger_clear(void)
{
    int ret;

    cpu_atomic_start();
    ret = -1;
    if (usb_mod_info.logger.attached == true) {
        ret = -2;
        if (usb_mod_info.logger.device.primitive.clear != NULL) {
            ret = usb_mod_info.logger.device.primitive.clear(
                &usb_mod_info.logger.device
            );
        }
    }
    cpu_atomic_end();

    return ret;
}

/* usb_logger_write() : printf-like text injection */
ssize_t usb_logger_write(char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);

    cpu_atomic_start();
    ret = -1;
    if (usb_mod_info.logger.attached == true) {
        ret = -2;
        if (usb_mod_info.logger.device.primitive.write != NULL)
            ret = usb_mod_info.logger.device.primitive.write(
                &usb_mod_info.logger.device,
                format,
                ap
            );
    }
    cpu_atomic_end();

    va_end(ap);

    return ret;
}

/* usb_logger_getline() : getline-like text fetch */
ssize_t usb_logger_getline(char **lineptr, size_t *n)
{
    int ret;

    cpu_atomic_start();
    ret = -1;
    if (usb_mod_info.logger.attached == true) {
        ret = -2;
        if (usb_mod_info.logger.device.primitive.getline != NULL)
            ret = usb_mod_info.logger.device.primitive.getline(
                &usb_mod_info.logger.device,
                lineptr,
                n
            );
    }
    cpu_atomic_end();

    return ret;
}

/* usb_logger_detach() : detach the logger */
int usb_logger_detach(void)
{
    int ret;

    cpu_atomic_start();
    ret = -1;
    if (usb_mod_info.logger.attached == true) {
        ret = -2;
        if (usb_mod_info.logger.device.primitive.quit != NULL)
            ret = usb_mod_info.logger.device.primitive.quit(
                &usb_mod_info.logger.device
            );
    }
    if (ret == 0) {
        memset(&usb_mod_info.logger.device, 0x00, sizeof(usb_logger_t));
        usb_mod_info.logger.attached = false;
    }
    cpu_atomic_end();

    return ret;
}
