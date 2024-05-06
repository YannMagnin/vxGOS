#include <SDL2/SDL.h>

/* Literal error message printed to stderr, evaluates to 1 for a combined
   return/exit() call */
#define err(fmt, ...) ({                                \
    fprintf(stderr, "error: " fmt "\n", ##__VA_ARGS__); \
    1;                                                  \
})


SDL_Window *__sdl_window = NULL;

__attribute__((constructor))
static int __sdl2_init(void)
{
    if(SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_TIMER))
        return 0;

    int rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if(rc < 0)
        return err("Cannot initialize SDL: %s\n", SDL_GetError());
    return 0;
}

__attribute__((destructor))
void __sdl_quit(void)
{
    if(__sdl_window != NULL) {
        SDL_DestroyWindow(__sdl_window);
        __sdl_window = NULL;
    }
}
