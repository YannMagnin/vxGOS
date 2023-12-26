#include <SDL2/SDL.h>
#include <vhex/display.h>
#include <vhex/display/interface.h>
#include <vhex/driver.h>


extern SDL_Window *__sdl_window;


//---
// Driver primitives
//---

uint32_t vhex_vram[(396*224) / 2];

/* sdl_frame_start() - prepar the screen and reset surfaces */
int sdl_frame_start(dsurface_t *surface)
{
    surface->vram = (void*)vhex_vram;
    surface->width = 396;
    surface->height = 224;
    surface->x1 = 0;
    surface->y1 = 0;
    surface->x2 = 395;
    surface->y2 = 223;
    return (0);
}

/* sdl_frame_frag_next() : update the next fragment */
int sdl_frame_frag_next(dsurface_t *surface)
{
    (void)surface;
    return -1;
}

/* sdl_frame_frag_send() : send fragment to the screen */
int sdl_frame_frag_send(dsurface_t *surface)
{

    SDL_SetWindowSize(__sdl_window, 396 * 2, 224 * 2);
    SDL_Surface *src = SDL_CreateRGBSurface(
        0, 396 * 2, 224 * 2, 24,
        0x000000ff, 0x0000ff00, 0x0000ff00, 0
    );

    uint16_t *vram = surface->vram;
    uint8_t *disp  = src->pixels;

    int r;
    int g;
    int b;

    off_t yoff = 0;
    for (int y = 0 ; y < 224; ++y) {
        for (int x = 0; x < 396 ; ++x) {
            r = (((vram[(y * 396) + x] & 0xf800) >> 11) * 255) / 31;
            g = (((vram[(y * 396) + x] & 0x07e0) >>  5) * 255) / 63;
            b = (((vram[(y * 396) + x] & 0x001f) >>  0) * 255) / 31;

            disp[yoff + (x * 6) + 0] = r;
            disp[yoff + (x * 6) + 1] = g;
            disp[yoff + (x * 6) + 2] = b;

            disp[yoff + (x * 6) + 3] = r;
            disp[yoff + (x * 6) + 4] = g;
            disp[yoff + (x * 6) + 5] = b;
        }
        memcpy(&disp[yoff + (396 * 6)], &disp[yoff], 396 * 6);
        yoff += 396 * 6;
        yoff += 396 * 6;
    }

    SDL_Surface *dst = SDL_GetWindowSurface(__sdl_window);
    SDL_BlitSurface(src, NULL, dst, NULL);
    SDL_FreeSurface(src);

    SDL_UpdateWindowSurface(__sdl_window);
    return 0;
}

/* sdl_frame_end() : destructor */
int sdl_frame_end(dsurface_t *surface)
{
    (void)surface;
    return 0;
}

//---
// Driver context management
//---

static int __sdl_configure(void)
{
    printf("[drv] LCD: fake driver initialized\n");
    __sdl_window = SDL_CreateWindow(
        "vxKernel",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        396 * 2,
        224 * 2,
        0
    );
    return 0;
}

static void __sdl_hsave(void)
{
    // Nothing to do, this is a fake driver
    ;
}

static void __sdl_hrestore(void)
{
    // Nothing to do, this is a fake driver
    ;
}


struct vhex_driver drv_sdl = {
    .name       = "SDL",
    .hsave      = (void*)&__sdl_hsave,
    .hrestore   = (void*)&__sdl_hrestore,
    .configure  = (void*)&__sdl_configure,
    .state_size = 4,
    .flags = {
        .DISPLAY = 1,
        .SHARED  = 0,
        .UNUSED  = 0,
    },
    .module_data = &(struct dstack_drv_interface){
        .frame_start     = &sdl_frame_start,
        .frame_frag_next = &sdl_frame_frag_next,
        .frame_frag_send = &sdl_frame_frag_send,
        .frame_end       = &sdl_frame_end,
        .display_width   = 396,
        .display_height  = 224
    }
};
VHEX_DECLARE_DRIVER(16, drv_sdl);
