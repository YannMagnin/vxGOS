//---
// modules:display:font:utf8    - utf8 font handling
//---

#include "vhex/modules/display/font.h"

//---
// Public
//---

/* dfont_utf8_next(): read the next UTF-8 code point of a string */
uint32_t dfont_utf8_next(uint8_t const **str_pointer)
{
    uint8_t const *str;
    uint8_t lead;
    uint8_t n2;
    uint8_t n3;
    uint8_t n4;

    lead = (*str_pointer)[0];
    str  = &((*str_pointer)[1]);

    /* Skip non-leaders which are invalid as starting bytes */
    while(
          (lead >= 0x80 && lead <= 0xbf)
        || lead == 0xc0
        || lead == 0xc1
        || lead == 0xfe
        || lead == 0xff
    ) {
        lead = str[0];
        str  = &str[1];
    }

    /* This base case will handle the NUL terminator */
    if(lead <= 0x7f) {
        *str_pointer = str;
        return lead;
    }

    n2 = str[0] & 0x3f;
    if(lead <= 0xdf) {
        *str_pointer = &str[1];
        return ((lead & 0x1f) << 6) | n2;
    }

    n3 = str[1] & 0x3f;
    if(lead <= 0xef) {
        *str_pointer = &str[2];
        return ((lead & 0x0f) << 12) | (n2 << 6) | n3;
    }

    n4 = str[2] & 0x3f;
    if(lead <= 0xf7) {
        *str_pointer = &str[3];
        return ((lead & 0x07) << 18) | (n2 << 12) | (n3 << 6) | n4;
    }

    /* It the string is too invalid, force a space and try to continue */
    *str_pointer = &str[4];
    return 0x20;
}
