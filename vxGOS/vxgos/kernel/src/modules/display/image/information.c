#if 0
#include <vhex/display/image.h>
#include <vhex/defs/utils.h>

/* image_valid(): Check if an image is valid */
bool image_valid(image_t const *img)
{
    if(img == NULL)
        return (false);

    if(IMAGE_IS_RGB16(img->format)) {
        return (img->data != NULL);
    }
    if(IMAGE_IS_P8(img->format) || IMAGE_IS_P4(img->format)) {
        return (
            (img->data != NULL)
            && (img->palette != NULL)
            && (img->color_count != 0)
        );
    }

    return (false);
}

/* image_alpha(): Get the alpha value for an image format */
int image_alpha(int format)
{
    switch(format)
    {
        case IMAGE_RGB565A:
            return (0x0001);
        case IMAGE_P8_RGB565A:
            return (0);
        case IMAGE_P4_RGB565A:
            return (0);
        default:
            /* A value that cannot be found in any pixel of any format */
            return (0x10000);
    }
}

/* image_get_pixel(): Read a pixel from the data array */
int image_get_pixel(image_t const *img, int x, int y)
{
    if((unsigned)x >= img->width || (unsigned)y >= img->height)
        return (0);

    void *data = img->data + (y * img->stride);
    uint8_t *data_u8 = data;
    uint16_t *data_u16 = data;

    if(IMAGE_IS_RGB16(img->format)) {
        return (data_u16[x]);
    }
    else if(IMAGE_IS_P8(img->format)) {
        return (data_u8[x]);
    }
    else if(IMAGE_IS_P4(img->format)) {
        if(x & 1) {
            return (data_u8[x >> 1] & 0x0f);
        } else {
            return (data_u8[x >> 1] >> 4);
        }
    }

    return (0);
}

/* image_decode_pixel(): Decode a pixel value */
int image_decode_pixel(image_t const *img, int pixel)
{
    if(IMAGE_IS_RGB16(img->format))
        return (pixel);
    else if(IMAGE_IS_P8(img->format))
        return (img->palette[pixel]);
    else if(IMAGE_IS_P4(img->format))
        return (img->palette[pixel]);
    return (-1);
}

/* image_data_size(): Compute the size of the [data] array */
int image_data_size(image_t const *img)
{
    return (img->stride * img->height);
}
#endif
