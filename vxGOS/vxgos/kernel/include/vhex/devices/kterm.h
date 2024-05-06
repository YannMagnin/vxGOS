#ifndef VHEX_DEVICE_KTERM_H
#define VHEX_DEVICE_KTERM_H 1

#include <stdint.h>

#include "vhex/defs/types.h"
#include "vhex/modules/display/interface.h"
#include "vhex/modules/display/types.h"

//--
// Internals
//---

/* magic watermark */
#define _KTERM_INIT_MAGIC   0xdeb0cad0

/* _kterm : internal device information */
struct _kterm
{
    struct {
        int x;
        int y;
    } cursor;
    byte_union(
        flags,
        uint8_t             :7;
        uint8_t NEED_SCROLL :1;
    );
    struct {
        struct dstack_drv_interface driver;
        struct dsurface surface;
    } display;
    uint32_t __magic;
};

//---
// Public
//---

/* kterm_init() : kterm initialisation */
extern int kterm_init(void);

/* kterm_clear() : clear screen and reset cursor position */
extern int kterm_clear(int color);

/* kterm_write() : display a format string */
extern int kterm_write(char const * const format, ...);

#endif /* VHEX_DEVICE_KTERM_H */
