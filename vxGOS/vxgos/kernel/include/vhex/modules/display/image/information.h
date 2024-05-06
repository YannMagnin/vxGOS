#ifndef __VHEX_DISPLAY_DRAW_IMAGE_INFORMATION__
# define __VHEX_DISPLAY_DRAW_IMAGE_INFORMATION__

#include <vhex/display/image/types.h>

//---
// Basic image access and information
//---

/* image_valid(): Check if an image is valid
   An image is considered valid if it has a valid profile, a non-NULL data
   pointer, and for palette formats a valid palette pointer. */
extern bool image_valid(image_t const *img);

/* image_alpha(): Get the alpha value for an image format

   This function returns the alpha value for any specific image format:
   * RGB16: 0x0001
   * P8: -128 (0x80)
   * P4: 0
   For non-transparent formats, it returns a value that is different from all
   valid pixel values of the format, which means it is always safe to compare a
   pixel value to the image_alpha() of the format. */
extern int image_alpha(int format);

/* image_get_pixel(): Read a pixel from the data array

   This function reads a pixel from the image's data array at position (x,y).
   It returns the pixel's value, which is either a full-color value (RGB16) or
   a possibly-negative palette index (P8/P4). See the description of the [data]
   field of image_t for more details. The value of the pixel can be decoded
   into a 16-bit color either manually or by using the image_decode_pixel()
   function.

   Note that reading large amounts of image data with this function will be
   slow; if you need reasonable performance, consider iterating on the data
   array manually. */
extern int image_get_pixel(image_t const *img, int x, int y);

/* image_decode_pixel(): Decode a pixel value

   This function decodes a pixel's value obtained from the data array (for
   instance with image_get_pixel()). For RGB16 formats this does nothing, but
   for palette formats this accesses the palette at a suitable position.

   Note that reading large amounts of data with this function will be slow; if
   you need reasonable performance, consider inlining the format-specific
   method or iterating on the data array manually. */
extern int image_decode_pixel(image_t const *img, int pixel);

/* image_data_size(): Compute the size of the [data] array
   This function returns the size of the data array, in bytes. This can be used
   to duplicate it. Note that for sub-images this is a subsection of another
   image's data array, and might be much larger than the sub-image. */
extern int image_data_size(image_t const *img);


#endif /* __VHEX_DISPLAY_DRAW_IMAGE_INFORMATION__ */
