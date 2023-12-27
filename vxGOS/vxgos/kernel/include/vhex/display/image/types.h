#ifndef __VHEX_DISPLAY_DRAW_IMAGE_TYPES__
# define __VHEX_DISPLAY_DRAW_IMAGE_TYPES__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>
#include <vhex/display/types.h>

/* Image formats. Note that transparency really only indicates the default
   rendering method, as a transparent background can always be added or removed
   by a dynamic effect (shader) on any image. */
enum {
    IMAGE_RGB565      = 0,  /* RGB565 without alpha */
    IMAGE_RGB565A     = 1,  /* RGB565 with one transparent color */
    IMAGE_P8_RGB565   = 4,  /* 8-bit palette, all opaque colors */
    IMAGE_P8_RGB565A  = 5,  /* 8-bit with one transparent color */
    IMAGE_P4_RGB565   = 6,  /* 4-bit palette, all opaque colors */
    IMAGE_P4_RGB565A  = 3,  /* 4-bit with one transparent color */
};

/* Quick macros to compare formats by storage size */
#define IMAGE_IS_RGB16(format)          \
    ((format) == IMAGE_RGB565 || (format) == IMAGE_RGB565A)
#define IMAGE_IS_P8(format)             \
    ((format) == IMAGE_P8_RGB565 || (format) == IMAGE_P8_RGB565A)
#define IMAGE_IS_P4(format)             \
    ((format) == IMAGE_P4_RGB565 || (format) == IMAGE_P4_RGB565A)
/* Check whether image format has an alpha color */
#define IMAGE_IS_ALPHA(format)          \
    ((format) == IMAGE_RGB565A    ||    \
     (format) == IMAGE_P8_RGB565A ||    \
     (format) == IMAGE_P4_RGB565A)
/* Check whether image format uses a palette */
#define IMAGE_IS_INDEXED(format)        \
    (IMAGE_IS_P8(format) || IMAGE_IS_P4(format))

/* Image flags. These are used for memory management, mostly. */
enum {
    IMAGE_FLAGS_DATA_RO         = 0x01,  /* Data is read-only */
    IMAGE_FLAGS_PALETTE_RO      = 0x02,  /* Palette is read-only */
    IMAGE_FLAGS_DATA_ALLOC      = 0x04,  /* Data is malloc()'d */
    IMAGE_FLAGS_PALETTE_ALLOC   = 0x08,  /* Palette is malloc()'d */
    IMAGE_FLAGS_RO              = 0x10,  /* image is const */
    IMAGE_FLAGS_OWN             = 0x20,  /* image is owned */
};

/* image_t: vhex's native bitmap image format
   Images of this format can be created through this header's API but also by
   using the `vxSDK conv` command. */
typedef struct
{
    /* Color format, one of the IMAGE_* values defined above */
    uint8_t format;
    /* Additional flags, a combination of IMAGE_FLAGS_* values */
    uint8_t flags;
    /* Number of colors in the palette; this includes alpha for transparent
       images, as alpha is always the first entry.
       RGB16: 0
       P8: Ranges between 1 and 256
       P4: 16 */
    int16_t color_count;
    /* Full width and height, in pixels */
    uint16_t width;
    uint16_t height;
    /* Byte stride between lines */
    int stride;

    /* Pixel data in row-major order, left to right.
       - RGB16: 2 bytes per entry, each row padded to 4 bytes for alignment.
         Each 2-byte value is an RGB565 color.
       - P8: 1 signed byte per entry. Each byte is a palette index shifted by
         128 (to access the color, use palette[<value>+128]).
       - P4: 4 bits per entry, each row padded to a full byte. Each entry is a
         direct palette index between 0 and 15. */
    void *data;

    /* For P8 and P4, color palette. The number of entries allocated in the
       array is equal to the color_count attribute. */
    uint16_t *palette;

} VPACKED(4) image_t;


#endif /* __VHEX_DISPLAY_DRAW_IMAGE_TYPES__ */
