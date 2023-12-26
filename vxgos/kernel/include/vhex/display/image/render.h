#ifndef __VHEX_DISPLAY_DRAW_IMAGE_RENDER__
# define __VHEX_DISPLAY_DRAW_IMAGE_RENDER__

#include <vhex/display/image/types.h>

//---
// User-level API
//---

/* render part */

/* Alignment settings for dimage*(). Combining a vertical and a horizontal
   alignment option specifies where a given point (x,y) should be relative to
   the rendered image. */
enum {
    /* Horizontal settings: default in diage*() is DIMAGE_LEFT */
    DIMAGE_LEFT   = 0x00,
    DIMAGE_CENTER = 0x01,
    DIMAGE_RIGHT  = 0x02,
    /* Vertical settings: default in dimage*() is DIMAGE_TOP */
    DIMAGE_TOP    = 0x10,
    DIMAGE_MIDDLE = 0x20,
    DIMAGE_BOTTOM = 0x40,
};

/* dimage(): Render a full image */
extern did_t dimage(image_t const *image, int x, int y, int mode);

/* dsubimage(): Render a section of an image */
extern did_t dsubimage(
    image_t const *image,
    int x, int y,
    int left, int top, int width, int height
);

/* shader part */

/* dimage_shader_shadow() : add shadows in any image rendered */
extern int dimage_shader_shadow(did_t did, int xoff, int yoff);

//---
// Kernel-level API
//---

/* dsubimage_render() : draw a subimage in the surface */
extern void dsubimage_render(
    dsurface_t *surface,
    image_t const *image,
    int x, int y,
    int left, int top, int width, int height
);

/* dimage_render(): Render a full image */
extern void dimage_render(dsurface_t *s, image_t const *img, int x, int y);

/* help part */

struct imgbox {
    struct {
        struct {
            int start;
            int end;
        } y;
        struct {
            int start;
            int end;
        } x;
    } img;
    struct {
        uint16_t *vram;
        int voff;
    } surface;
};

/* dsubimage_render_box() : generate box information */
extern int dsubimage_render_box(
    struct imgbox *imgbox,
    dsurface_t *surface,
    image_t const *image,
    int x, int y,
    int left, int top, int width, int height
);

#endif /* __VHEX_DISPLAY_DRAW_IMAGE_RENDER__ */
