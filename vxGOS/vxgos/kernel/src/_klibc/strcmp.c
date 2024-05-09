//---
// _klibc:memcpy    - kernel internal memcpy
//---

#include "vhex/_klibc.h"

//---
// Public
//---

/* klibc_strncmp() : compare two strings */
int klibc_strncmp(char const *s1, char const *s2, size_t n)
{
    size_t counter;

    counter = 0;
    while (
           counter < n - 1
        && s1[0] != '\0'
        && s2[0] != '\0'
        && s1[0] == s2[0]
    ) {
        s1 = &s1[1];
        s2 = &s2[1];
        counter += 1;
    }
    return ((unsigned char) s1[0] - (unsigned char) s2[0]);
}

/* klibc_strcmp() : compare two strings */
int klibc_strcmp(char const *s1, char const *s2)
{
    while (
           s1[0] != '\0'
        && s2[0] != '\0'
        && s1[0] == s2[0]
    ) {
        s1 = &s1[1];
        s2 = &s2[1];
    }
    return ((unsigned char) s1[0] - (unsigned char) s2[0]);
}
