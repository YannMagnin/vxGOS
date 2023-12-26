#ifndef __VHEX_USB_LOGGER__
#define __VHEX_USB_LOGGER__ 1

#include <stdarg.h>
#include <vhex/defs/types.h>

/* usb_logger : USB logger information */
struct usb_logger {

    /* logger primitive (can be NULL)
     * - init()     : initialize buffer and internal information
     * - clear()    : clear the content of the buffer (reset)
     * - write()    : inject a text in the buffer
     * - getlines() : fetch the line starting at `*lineptr`
     * - quit()     : uninitialize buffer and internal information */
    struct {
        int (*init)(struct usb_logger *l, size_t buffer_size);
        int (*clear)(struct usb_logger *l);
        ssize_t (*write)(struct usb_logger *l, char *text, va_list ap);
        ssize_t (*getline)(struct usb_logger *l, char **lineptr, size_t *n);
        int (*quit)(struct usb_logger *logger);
    } primitive;

    /* buffer geometry information
     * - buffer.data         : buffer data
     * - buffer.size         : buffer size
     * - buffer.inject_pos   : text injection position
     * */
    struct {
        char *data;
        size_t size;
        int pos;
    } buffer;

    /* _private : user-provided information */
    void *_private;
};
typedef struct usb_logger usb_logger_t;


//---
// Public logger API
//---

/* usb_logger_set() : return true if a logger is attached */
extern bool usb_logger_is_attached(void);

/* usb_logger_set() : attach a logger to the kernel USB driver (clear)*/
extern int usb_logger_attach(usb_logger_t *logger, size_t buffer_size);

/* usb_logger_clear() : clear the logger buffer */
extern int usb_logger_clear(void);

/* usb_logger_write() : printf-like text injection */
extern ssize_t usb_logger_write(char *format, ...);

/* usb_logger_getline() : getline-like text fetch */
extern ssize_t usb_logger_getline(char **lineptr, size_t *n);

/* usb_logger_detach() : detach the logger */
extern int usb_logger_detach(void);

#endif /* __VHEX_USB_LOGGER__ */
