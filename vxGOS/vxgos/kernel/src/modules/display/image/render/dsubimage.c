#if 0
#include <vhex/display/image.h>
#include <vhex/display/shader.h>
#include <vhex/display.h>

//---
// Internal functions
//---


/* dsubimage_default_render() : raw-format image render */
static void dsubimage_default_render(image_t const *image, struct imgbox *box)
{
    uint16_t *vram;
    uint16_t *data;
    int voff;
    int c;

    voff = box->surface.voff;
    vram = box->surface.vram;

    for (int iy = box->img.y.start; iy < box->img.y.end; ++iy) {
        data = vram;
        c = 0;
        for (int ix = box->img.x.start; ix < box->img.x.end; ++ix) {
            data[c] = image_decode_pixel(
                image,
                image_get_pixel(image, ix, iy)
            );
            c += 1;
        }
        vram = &vram[voff];
    }
}

/* dsubimage_alpha_render() : alpha-format image render */
static void dsubimage_alpha_render(image_t const *image, struct imgbox *box)
{
    uint16_t *vram;
    uint16_t *data;
    uint16_t pixel;
    uint16_t alpha;
    int voff;
    int c;

    voff = box->surface.voff;
    vram = box->surface.vram;
    alpha = image_alpha(image->format);

    for (int iy = box->img.y.start; iy < box->img.y.end; ++iy) {
        data = vram;
        c = 0;
        for (int ix = box->img.x.start; ix < box->img.x.end; ++ix) {
            pixel = image_get_pixel(image, ix, iy);
            if (pixel != alpha)
                data[c] = image_decode_pixel(image, pixel);
            c += 1;
        }
        vram = &vram[voff];
    }
}


//---
// Kernel-level API
//---


/* dsubimage_render_box() : generate box information */
int dsubimage_render_box(
    struct imgbox *imgbox,
    dsurface_t *surface,
    image_t const *image,
    int x, int y,
    int left, int top, int width, int height
) {
    uint16_t *vram;
    int vxs;
    int vys;

    //TODO: precalculate image information
    (void)image;

    /* precalculate culling information */
    if (x < 0) {
        left  -= x;
        width += x;
        x = 0;
    }
    if (y < 0) {
        top    -= y;
        height += y;
        y = 0;
    }

    /* check error */
    if (top < 0 || left < 0 || width < 0 || height < 0)
        return -1;

    /* check culling */
    if (y >= surface->y2
        || y + height < surface->y1
        || x >= surface->x2
        || x + width < surface->x1) {
        return -2;
    }

    /* precalculate geometry information (both image and vram) */
    vxs = x - surface->x1;
    vys = y - surface->y1;
    if (vxs < 0) {
        vxs    = 0;
        left  += (surface->x1 - x);
        width -= (surface->x1 - x);
    }
    if (vys < 0) {
        vys     = 0;
        top    += (surface->y1 - y);
        height -= (surface->y1 - y);
    }
    if ((size_t)(vxs + width)  > surface->width)
        width  = surface->width - vxs;
    if ((size_t)(vys + height) > surface->height)
        height = surface->height - vys;
    vram = surface->vram;

    /* prepare box request */
    imgbox->img.y.start = top;
    imgbox->img.y.end   = top + height;
    imgbox->img.x.start = left;
    imgbox->img.x.end   = left + width;
    imgbox->surface.vram = &vram[(surface->width * vys) + vxs],
    imgbox->surface.voff = surface->width;
    return 0;
}

/* dsubimage_render() : draw a subimage in the surface */
void dsubimage_render(
    dsurface_t *surface,
    image_t const *image,
    int x, int y,
    int left, int top, int width, int height
) {
    struct imgbox imgbox;
    int ret;

    ret = dsubimage_render_box(
        &imgbox,
        surface,
        image,
        x, y,
        left, top, width, height
    );
    if (ret != 0)
        return;

#if 0
    static int test = 0;
    if (++test >= 0) {
        dclear(C_WHITE);
        dprint(0, 0, C_BLACK,
            "surface.x1 = %d\n"
            "surface.x2 = %d\n"
            "surface.y1 = %d\n"
            "surface.y2 = %d\n"
            "surface.width = %d\n"
            "surface.height = %d\n"
            "\n"
            "img.y.start = %d\n"
            "img.y.end   = %d\n"
            "img.x.start = %d\n"
            "img.x.end   = %d\n"
            "\n"
            "image.x = %d\n"
            "image.y = %d\n"
            "image.width = %d\n"
            "image.heigt = %d\n",
            surface->x1,
            surface->x2,
            surface->y1,
            surface->y2,
            surface->width,
            surface->height,
            imgbox.img.y.start,
            imgbox.img.y.end,
            imgbox.img.x.start,
            imgbox.img.x.end,
            x,
            y,
            width,
            height
        );
        dupdate();
        while(1) { __asm__ volatile ("sleep"); }
    }
#endif

    /* send drawing request in the approriate optimized function */
    if (IMAGE_IS_ALPHA(image->format)) {
        dsubimage_alpha_render(image, &imgbox);
        return;
    }
    dsubimage_default_render(image, &imgbox);
}




//---
// Dstack API
//---

/* dsubimage_dstack(): dstack wrapper primitive */
void dsubimage_dstack(dsurface_t *surface, uintptr_t *args)
{
    dsubimage_render(
        surface,
        (image_t*)args[0],
        (int)args[1],
        (int)args[2],
        (int)args[3],
        (int)args[4],
        (int)args[5],
        (int)args[6]
    );
}



//---
// User-level API
//---

/* dsubimage(): Render a section of an image */
did_t dsubimage(
    image_t const *image,
    int x, int y,
    int left, int top, int width, int height
) {
    return dstack_add_action(
        DSTACK_CALL(
            &dsubimage_dstack,
            (uintptr_t)image,
            x, y,
            left, top, width, height
        ),
        NULL,
        NULL
    );
}
#endif
