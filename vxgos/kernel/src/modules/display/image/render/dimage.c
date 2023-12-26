#include <vhex/display/image.h>
#include <vhex/display/stack.h>


//---
// Kernel-level API
//---

/* dimage_render(): Render a full image */
void dimage_render(dsurface_t *surface, image_t const *img, int x, int y)
{
    dsubimage_render(surface, img, x, y, 0, 0, img->width, img->height);
}

//---
// User-level API
//---

/* dimage(): Render a full image */
did_t dimage(image_t const *image, int x, int y, int mode)
{
    extern void dsubimage_dstack(dsurface_t *surface, uintptr_t *args);

    if (mode & DIMAGE_CENTER) { x -= image->width >> 1; }
    if (mode & DIMAGE_RIGHT)  { x -= image->width; }
    if (mode & DIMAGE_MIDDLE) { y -= image->height >> 1; }
    if (mode & DIMAGE_BOTTOM) { y -= image->height; }
    return dstack_add_action(
        DSTACK_CALL(
            &dsubimage_dstack,
            (uintptr_t)image,
            x, y,
            0, 0, image->width, image->height
        ),
        NULL,
        NULL
    );
}
