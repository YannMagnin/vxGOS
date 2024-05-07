#if 0
#include <vhex/display/image.h>
#include <vhex/display/types.h>
#include <vhex/display/stack.h>
#include <vhex/display/shader.h>

//---
// Internal functions
//---

/* dimg_shadow_default_render() : add shadow on image with no alpha */
static void dimg_shadow_default_render(
    image_t *image,
    struct imgbox *box,
    int xoff,
    int yoff
) {
    (void)image;
    (void)box;
    (void)xoff;
    (void)yoff;
}



/* dimg_shadow_alpha_render() : add shadow on image with no alpha */
static void dimg_shadow_alpha_render(
    image_t *image,
    struct imgbox *box,
    int xoff,
    int yoff
) {
    uint16_t *vram;
    uint16_t *data;
    uint16_t pixel;
    uint16_t alpha;
    int voff;
    int xcnt;
    int ycnt;
    int c;
    int msk;
    int pxl;

    (void)xoff;
    (void)yoff;

    voff = box->surface.voff;
    vram = box->surface.vram;
    alpha = image_alpha(image->format);



    if (yoff < 0)
    {
        ycnt = 4;
        if (box->img.y.start <= 0)
            ycnt = 0;
        for (int iy = box->img.y.start; iy < box->img.y.end; ++iy) {
            data = vram;
            xcnt = 0;
            if (xoff < 0) {
                c = 0;
                for (int ix = box->img.x.start; ix < box->img.x.end; ++ix) {
                    pixel = image_get_pixel(image, ix, iy);
                    if (pixel != alpha) {
                        if (xcnt < 4 || ycnt < 4) {
                            pxl = data[c];
                            msk = pxl;
                            pxl  = ((msk & 0xF81F) >> 1) & 0xF81F;
                            pxl |= ((msk & 0x07E0) >> 1) & 0x07E0;
                            data[c] = pxl;
                            xcnt += 1;
                        }
                    } else {
                        xcnt = 0;
                    }
                    c += 1;
                }
            } else {
                c = box->img.x.end - box->img.x.start - 1;
                for (int ix = box->img.x.end - 1; ix >= box->img.x.start; --ix) {
                    pixel = image_get_pixel(image, ix, iy);
                    if (pixel != alpha) {
                        if (xcnt < 4 || ycnt < 4) {
                            pxl = data[c];
                            msk = pxl;
                            pxl  = ((msk & 0xF81F) >> 1) & 0xF81F;
                            pxl |= ((msk & 0x07E0) >> 1) & 0x07E0;
                            data[c] = pxl;
                            xcnt += 1;
                        }
                    } else {
                        xcnt = 0;
                    }
                    c -= 1;
                }
            }
            vram = &vram[voff];
            ycnt += 1;
        }
    } else {
        ycnt = 4;
        if (box->img.y.end >= image->height)
            ycnt = 0;
        vram = &vram[voff * (box->img.y.end - box->img.y.start - 1)];
        for (int iy = box->img.y.end - 1; iy >= box->img.y.start; --iy) {
            data = vram;
            xcnt = 0;
            if (xoff < 0) {
                c = 0;
                for (int ix = box->img.x.start; ix < box->img.x.end; ++ix) {
                    pixel = image_get_pixel(image, ix, iy);
                    if (pixel != alpha) {
                        if (xcnt < 4 || ycnt < 4) {
                            pxl = data[c];
                            msk = pxl;
                            pxl  = ((msk & 0xF81F) >> 1) & 0xF81F;
                            pxl |= ((msk & 0x07E0) >> 1) & 0x07E0;
                            data[c] = pxl;
                            xcnt += 1;
                        }
                    } else {
                        xcnt = 0;
                    }
                    c += 1;
                }
            } else {
                c = box->img.x.end - box->img.x.start - 1;
                for (int ix = box->img.x.end - 1; ix >= box->img.x.start; --ix) {
                    pixel = image_get_pixel(image, ix, iy);
                    if (pixel != alpha) {
                        if (xcnt < 4 || ycnt < 4) {
                            pxl = data[c];
                            msk = pxl;
                            pxl  = ((msk & 0xF81F) >> 1) & 0xF81F;
                            pxl |= ((msk & 0x07E0) >> 1) & 0x07E0;
                            data[c] = pxl;
                            xcnt += 1;
                        }
                    } else {
                        xcnt = 0;
                    }
                    c -= 1;
                }
            }
            vram = &vram[0 - voff];
            ycnt += 1;
        }
    }
}


//---
// Dstack-level API
//---

#include <vhex/display.h>

/* dimage_shader_shadow() : add shadows in any image rendered */
static void dimage_shader_shadow_dstack(
    dsurface_t *surface,
    uintptr_t *draw,
    uintptr_t *shader
) {
    struct imgbox imgbox;
    image_t *image;
    int xoff;
    int yoff;
    int ret;

    image = (image_t*)draw[0];
    xoff = (int)shader[0];
    yoff = (int)shader[1];

    ret = dsubimage_render_box(
        &imgbox,
        surface,
        image,
        (int)draw[1] + xoff,
        (int)draw[2] + yoff,
        (int)draw[3],
        (int)draw[4],
        (int)draw[5],
        (int)draw[6]
    );
    if (ret != 0)
        return;

#if 0
    static int test = 0;
    if (++test >= 2) {
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
            "image.heigt = %d\n"
            "image.ptr = %p\n",
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
            xoff,
            yoff,
            (int)draw[3],
            (int)draw[4],
            (int)draw[5],
            (int)draw[6],
            (image_t*)(uintptr_t)draw[0]
        );
        dupdate();
        while(1) { __asm__ volatile ("sleep"); }
    } else {
        return;
    }
#endif

    /* send drawing request in the approriate optimized function */
    if (IMAGE_IS_ALPHA(image->format)) {
        dimg_shadow_alpha_render(image, &imgbox, xoff, yoff);
        return;
    }
    dimg_shadow_default_render(image, &imgbox, xoff, yoff);
}

//---
// User-level API
//---

/* dimage_shader_shadow() : add shadows in any image rendered */
int dimage_shader_shadow(did_t did, int xoff, int yoff)
{
    return dstack_add_shader(
        did,
        DSHADER(&dimage_shader_shadow_dstack, xoff, yoff)
    );
}
#endif
