#if 0
#include <vhex/display/image.h>
#include <vhex/defs/utils.h>

#include <stdlib.h>
#include <string.h>

//---
// Image-object API
//---

/* image_alloc(): Create a new (uninitialized) image */
image_t *image_alloc(size_t width, size_t height, int format)
{
    image_t *img = image_create(width, height, format);
    if(img == NULL)
        return (NULL);

    if(IMAGE_IS_RGB16(format))
        img->stride = ((width + 1) >> 1) * 4;
    else if(IMAGE_IS_P8(format))
        img->stride = width;
    else if(IMAGE_IS_P4(format))
        img->stride = ((width + 1) >> 1);

    void *data = malloc(height * img->stride);
    if(data == NULL) {
        image_free(img);
        return (NULL);
    }

    img->data = data;
    img->flags |= IMAGE_FLAGS_DATA_ALLOC;
    return img;
}

/* image_create(): Create a bare image with no data/palette */
image_t *image_create(size_t width, size_t height, int format)
{
    if(!IMAGE_IS_RGB16(format)
            && !IMAGE_IS_P8(format)
            && !IMAGE_IS_P4(format)) {
        return (NULL);
    }
    if(width > 0xffff || height > 0xffff)
        return (NULL);

    image_t *img = calloc(1, sizeof(image_t));
    if(img == NULL)
        return (NULL);

    img->format = format;
    img->flags = 0;
    img->color_count = 0;
    img->width = width;
    img->height = height;
    img->stride = 0;
    img->data = NULL;
    img->palette = NULL;

    return (img);
}

/* image_free(): Free and image and the data it owns */
void image_free(image_t *img)
{
    if(img == NULL)
        return;

    if(img->flags & IMAGE_FLAGS_DATA_ALLOC)
        free(img->data);
    if(img->flags & IMAGE_FLAGS_PALETTE_ALLOC)
        free(img->palette);

    if (!(img->flags & IMAGE_FLAGS_RO))
        free(img);
}

//---
// Palette API
//---

/* image_set_palette(): Specify an external palette for an image */
void image_set_palette(image_t *img, uint16_t *palette, size_t size, bool owns)
{
    if(img == NULL || !IMAGE_IS_INDEXED(img->format))
        return;

    if(img->flags & IMAGE_FLAGS_PALETTE_ALLOC)
        free(img->palette);

    img->palette = palette;
    img->color_count = size;

    if(owns) {
        img->flags |= IMAGE_FLAGS_PALETTE_ALLOC;
    } else {
        img->flags &= ~IMAGE_FLAGS_PALETTE_ALLOC;
    }
}


/* image_alloc_palette(): Allocate a new palette for an image */
bool image_alloc_palette(image_t *img, int size)
{
    if(img == NULL || !IMAGE_IS_INDEXED(img->format))
        return (false);

    if(img->flags & IMAGE_FLAGS_PALETTE_ALLOC)
        free(img->palette);

    if(IMAGE_IS_P8(img->format)) {
        size = (size <= 0) ? 256 : min(size, 256);
    }
    if(IMAGE_IS_P4(img->format)) {
        size = 16;
    }

    img->palette = calloc(size, sizeof(uint16_t));
    img->color_count = 0;
    img->flags &= ~IMAGE_FLAGS_PALETTE_ALLOC;

    if(img->palette == NULL)
        return (false);

    memset(img->palette, 0, size * sizeof(uint16_t));
    img->color_count = size;
    img->flags |= IMAGE_FLAGS_PALETTE_ALLOC;
    return (true);
}

/* image_copy_palette(): Copy another image's palette */
bool image_copy_palette(image_t const *src, image_t *dst, int size)
{
    if(!image_valid(src) || dst == NULL)
        return (false);

    if(!IMAGE_IS_INDEXED(dst->format))
        return (true);

    if(size < 0)
        size = src->color_count;
    if(!image_alloc_palette(dst, size))
        return (false);

    int N = min(src->color_count, dst->color_count);
    memcpy(dst->palette, src->palette, N * sizeof(uint16_t));
    return (true);
}
#endif
