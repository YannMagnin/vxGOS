#ifndef __VHEX_DEFS_TYPES__
# define __VHEX_DEFS_TYPES__

#include <vhex/defs/attributes.h>

/* For size_t, mainly */
#include <stddef.h>
/* For all fixed-width integer types */
#include <stdint.h>
/* For human-readable boolean types */
#include <stdbool.h>
/* Common system types: ssize_t, off_t, etc. */
#include <sys/types.h>

/* Fixed-width types for bit fields are quite meaningless */
typedef unsigned int uint;

//---
//  Structure elements
//----

/* Giving a type to padding bytes is misguiding, let's hide it in a macro */
#define pad_nam2(c) _ ## c
#define pad_name(c) pad_nam2(c)
#define pad(bytes)  uint8_t pad_name(__COUNTER__)[bytes]

/* byte_union() - union between an uint8_t 'byte' element and a bit field */
#define byte_union(name, fields)        \
    union {                             \
        uint8_t byte;                   \
        struct { fields } VPACKED(1);   \
    } VPACKED(1) name

/* word_union() - union between an uint16_t 'word' element and a bit field */
#define word_union(name, fields)        \
    union {                             \
        uint16_t word;                  \
        struct { fields } VPACKED(2);   \
    } VPACKED(2) name

/* lword_union() - union between an uint32_t 'lword' element and a bit field */
#define lword_union(name, fields)       \
    union {                             \
        uint32_t lword;                 \
        struct { fields } VPACKED(4);   \
    } VPACKED(4) name


/* byte_union() - same as `byte_union()` but without aligment */
#define byte_union_alone(name, fields)  \
    union {                             \
        uint8_t byte;                   \
        struct { fields } VPACKED_ALONE;\
    } VPACKED_ALONE name

/* word_union_alone() - same as `word_union()` but without aligment */
#define word_union_alone(name, fields)  \
    union {                             \
        uint16_t word;                  \
        struct { fields } VPACKED_ALONE;\
    } VPACKED_ALONE name

/* lword_union_alone() - same as `lword_union()` but without aligment */
#define lword_union_alone(name, fields) \
    union {                             \
        uint32_t lword;                 \
        struct { fields } VPACKED_ALONE;\
    } VPACKED_ALONE name

#endif /*__VHEX_DEFS_TYPES__*/
