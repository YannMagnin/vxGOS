#ifndef __VHEX_DISPLAY_DRAW_IMAGE_OBJECT__
# define __VHEX_DISPLAY_DRAW_IMAGE_OBJECT__

#include <vhex/display/image/types.h>

//---
// Image creation and destruction
//---

/* image_alloc(): Create a new (uninitialized) image

   This function allocates a new image of the specified dimensions and format.
   It always allocates a new data array; if you need to reuse a data array, use
   the lower-level image_create() or image_create_sub().

   The first parameters [width] and [height] specify the dimensions of the new
   image in pixels. The [format] should be one of the IMAGE_* formats, for
   example IMAGE_RGB565A or IMAGE_P4_RGB565.

   This function does not specify or initialize the palette of the new image;
   use image_set_palette(), image_alloc_palette() or image_copy_palette()
   after calling this function.

   The returned image structure must be freed with image_free() after use.

   @width         Width of the new image
   @height        Height of the new image
   @format        Pixel format; one of the IMAGE_* formats defined above */
extern image_t *image_alloc(size_t width, size_t height, int format);

/* image_set_palette(): Specify an external palette for an image

   This function sets the image's palette to the provided address. The number
   of entries allocated must be specified in size. It is also the caller's
   responsibility to ensure that the palette covers all the indices used in the
   image data.

   The old palette, if owned by the image, is freed. */
extern void image_set_palette(image_t *img, uint16_t *p, size_t sz, bool owns);

/* image_alloc_palette(): Allocate a new palette for an image

   This function allocates a new palette for an image. The number of entries is
   specified in size; for P8 it can vary between 1 and 256, for P4 it is
   ignored (P4 images always have 16 colors).

   The old palette, if owned by the image, is freed. The entries of the new
   palette are all initialized to 0. If size is -1, the format's default
   palette size is used. Returns true on success. */
extern bool image_alloc_palette(image_t *img, int size);

/* image_copy_palette(): Copy another image's palette

   This function allocates a new palette for an image, and initializes it with
   a copy of another image's palette. For P8 the palette can be resized by
   specifying a value other than -1 as the size; by default, the source image's
   palette size is used (within the limits of the new format). Retuns true on
   success. */
extern bool image_copy_palette(image_t const *src, image_t *dst, int size);

/* image_create(): Create a bare image with no data/palette

   This function allocates a new image structure but without data or palette.
   The [data] and [palette] members are NULL, [color_count] and [stride] are 0.

   This function is useful to create images that reuse externally-provided
   information. It is intended that the user of this function sets the [data]
   and [stride] fields themselves, along with the IMAGE_FLAGS_DATA_ALLOC flag
   if the image should own its data.

   The [palette] and [color_count] members can be set with image_set_palette(),
   image_alloc_palette(), image_copy_palette(), or manually.

   The returned image structure must be freed with image_free() after use. */
extern image_t *image_create(size_t width, size_t height, int format);

/* image_free(): Free and image and the data it owns

   This function frees the provided image structure and the data that it owns.
   Images converted by fxconv should not be freed; nonetheless, this functions
   distinguishes them and should work. Images are not expected to be created on
   the stack.

   If the image has the IMAGE_FLAGS_DATA_ALLOC flag, the data pointer is also
   freed. Similarly, the image has the IMAGE_FLAGS_PALETTE_ALLOC flag, the
   palette is freed. Make sure to not free images when references to them still
   exist, as this could cause the reference's pointers to become dangling. */
extern void image_free(image_t *img);


#endif /* __VHEX_DISPLAY_DRAW_IMAGE_OBJECT__ */
