#ifndef VHEX_KLIBC_H
#define VHEX_KLIBC_H 1

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

//---
// Public
//---


// string-family


/* klibc_strncmp() : compare two strings */
extern int klibc_strncmp(char const *s1, char const *s2, size_t n);

/* klibc_strcmp() : compare two strings */
extern int klibc_strcmp(char const *s1, char const *s2);

/* memset() : wipe arena */
extern void *klibc_memset(void *s, int c, size_t n);

/* memcpy() : copy memory arena */
extern void *klibc_memcpy(void *dest, const void *src, size_t count);


// printf-family


/* klibc_snprintf() : snprintf-like function */
extern int klibc_snprintf(char *buffer, size_t sz, char const *format, ...);

/* klibc_vsnprintf() : vsnprintf-like function */
extern int klibc_vsnprintf(
    char *buffer,
    size_t size,
    char const *format,
    va_list ap
);

#endif /* VHEX_KLIBC_H */
