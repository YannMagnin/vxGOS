#ifndef __VHEX_DISPLAY_FONT_TYPES__
# define __VHEX_DISPLAY_FONT_TYPES__

#include <vhex/defs/types.h>
#include <vhex/defs/attributes.h>

/* font_t: Font data encoded for dfont*() functions */
typedef struct font {
    /* Font name (NUL-terminated), NULL if no title */
    char const *name;

    /* Font shape flags */
    byte_union(shape,
        uint8_t bold    :1;
        uint8_t italic  :1;
        uint8_t serif   :1;
        uint8_t mono    :1;
        uint8_t         :3;
        uint8_t prop    :1;
    );

    /* Character spacing (usually 1) */
    uint8_t char_spacing;

    /* glyph information */
    struct {
        uint8_t height;
        uint8_t line_height;
        uint32_t *data;
        uint32_t count;

        union {
            /* For monospaced fonts */
            struct {
                uint8_t width;
                uint16_t storage_size;
            } mono;
            /* For proportional fonts */
            struct __workaround {
                uint8_t width;
                uint16_t index;
                uint8_t shift;
            } *prop;
        };
    } glyph;

    /* unicode blocks */
    struct {
        struct {
            /* Unicode point of first character in block */
            uint32_t start   :20;
            /* Length of block */
            uint32_t length  :12;
        } *blocks;
        uint8_t block_count;
    } unicode;

} VPACKED(4) font_t;

#endif /* __VHEX_DISPLAY_FONT_TYPES__ */
