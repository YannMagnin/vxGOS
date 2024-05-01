#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//---
// Internals
//---

struct __snprintf_core
{
    uintptr_t buffer;
    uintptr_t buffer_saved;
    size_t    buffer_size;
};

/* __buffer_update() : inject one char if possible */
static int __buffer_inj_char(struct __snprintf_core *core, char c)
{
    ((char*)core->buffer)[0] = c;
    core->buffer += 1;
    ((char*)core->buffer)[0] = '\0';
    if ((core->buffer - core->buffer_saved) < core->buffer_size - 1)
        return 0;
    return -1;
}

/* __buffer_inj_str() : inject string */
static int __buffer_inj_str(
    struct __snprintf_core *core,
    char const * const text
) {
    int ret;

    for (int i = 0 ; text[i] != '\0' ; ++i)
    {
        ret = __buffer_inj_char(core, text[i]);
        if (ret != 0)
            return ret;
    }
    return 0;
}

/* __buffer_inj_ptr() : inject ptr */
static int __buffer_inj_ptr(struct __snprintf_core *core, uintptr_t ptr)
{
    static char const * const table = "0123456789abcdef";

    if (
           __buffer_inj_char(core, '0') != 0
        || __buffer_inj_char(core, 'x') != 0
        || __buffer_inj_char(core, table[(ptr & 0xf0000000) >> 28]) != 0
        || __buffer_inj_char(core, table[(ptr & 0x0f000000) >> 24]) != 0
        || __buffer_inj_char(core, table[(ptr & 0x00f00000) >> 20]) != 0
        || __buffer_inj_char(core, table[(ptr & 0x000f0000) >> 16]) != 0
        || __buffer_inj_char(core, table[(ptr & 0x0000f000) >> 12]) != 0
        || __buffer_inj_char(core, table[(ptr & 0x00000f00) >>  8]) != 0
        || __buffer_inj_char(core, table[(ptr & 0x000000f0) >>  4]) != 0
        || __buffer_inj_char(core, table[(ptr & 0x0000000f) >>  0]) != 0
    ) {
        return -1;
    }
    return 0;
}

/* __buffer_inj_hex() : display hex */
static int __buffer_inj_hex(struct __snprintf_core *core, uint32_t data)
{
    char buffer[10];
    int ret;
    int tempo;
    int n;
    int i;

    i = 0;
    do {
        tempo = data & 0x0f;
        if (tempo >= 10) {
            n = 'A' + (tempo - 10);
        } else {
            n = '0' + tempo;
        }
        buffer[i] = n;
        data = data >> 4;
        i += 1;
    } while(data != 0x00000000);
    i -= 1;
    while (i >= 0) {
        ret = __buffer_inj_char(core, buffer[i]);
        if (ret != 0)
            return ret;
        i -= 1;
    }
    return 0;
}

/* __buffer_inj_digit() : inject number */
static int __buffer_inj_digit(
    struct __snprintf_core *core,
    int nb,
    bool patch
) {
    if (patch && nb == 0)
        return 0;
    if (__buffer_inj_digit(core, nb / 10, true) != 0)
        return -1;
    return __buffer_inj_char(core, (nb % 10) + '0');
}

//---
// Public
//---

/* vsnprintf() : snprintf-like function */
int vsnprintf(char *buffer, size_t sz, char const *format, va_list ap)
{
    struct __snprintf_core core;
    int exit;

    core.buffer       = (uintptr_t)buffer;
    core.buffer_saved = (uintptr_t)buffer;
    core.buffer_size  = sz;

    exit = 0;
    while (format[0] != '\0' && exit == 0)
    {
        if (format[0] != '%') {
            exit = __buffer_inj_char(&core, format[0]);
            format = &format[1];
            continue;
        }
        switch (format[1])
        {
            case '\0':
                exit = __buffer_inj_char(&core, '%') - 10;
                break;
            case '%':
                exit = __buffer_inj_char(&core, '%');
                break;
            case 'x':
                exit = __buffer_inj_hex(&core, va_arg(ap, uintptr_t));
                break;
            case 'p':
                exit = __buffer_inj_ptr(&core, va_arg(ap, uintptr_t));
                break;
            case 'd':
                exit = __buffer_inj_digit(&core, va_arg(ap, int), false);
                break;
            case 's':
                exit = __buffer_inj_str(&core, va_arg(ap, char *));
                break;
            default:
                exit = __buffer_inj_char(&core, format[0]);
                exit = __buffer_inj_char(&core, format[1]);
        }
        format = &format[2];
    }
    return (core.buffer - core.buffer_saved);
}

/* snprintf() : snprintf-like function */
int snprintf(char *buffer, size_t sz, char const *format, ...)
{
    va_list ap;
    int n;

    va_start(ap, format);
    n = vsnprintf(buffer, sz, format, ap);
    va_end(ap);
    return n;
}
