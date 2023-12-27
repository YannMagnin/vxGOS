#include <vhex/keyboard/interface.h>
#include <vhex/driver.h>
#include <vhex/keyboard.h>
#include <SDL2/SDL.h>

//---
// fake interrupt handler
//---

static struct {
    int sdl2_id;
    char const * const sdl2_name;
    int vhex_id;
    char const * const vhex_name;
} key_translation[] = {
    /* F-keys */
    {
        .sdl2_id = SDLK_F1,         .sdl2_name = "F1",
        .vhex_id = VKEY_F1,         .vhex_name = "F1"
    },
    {
        .sdl2_id = SDLK_F2,         .sdl2_name = "F2",
        .vhex_id = VKEY_F2,         .vhex_name = "F2"
    },
    {
        .sdl2_id = SDLK_F3,         .sdl2_name = "F3",
        .vhex_id = VKEY_F3,         .vhex_name = "F3"
    },
    {
        .sdl2_id = SDLK_F4,         .sdl2_name = "F4",
        .vhex_id = VKEY_F4,         .vhex_name = "F4"
    },
    {
        .sdl2_id = SDLK_F5,         .sdl2_name = "F5",
        .vhex_id = VKEY_F5,         .vhex_name = "F5"
    },
    {
        .sdl2_id = SDLK_F6,         .sdl2_name = "F6",
        .vhex_id = VKEY_F6,         .vhex_name = "F6"
    },

    /* Numeric keys */
    {
        .sdl2_id = SDLK_1,          .sdl2_name = "1",
        .vhex_id = VKEY_1,          .vhex_name = "1"
    },
    {
        .sdl2_id = SDLK_2,          .sdl2_name = "2",
        .vhex_id = VKEY_2,          .vhex_name = "2"
    },
    {
        .sdl2_id = SDLK_3,          .sdl2_name = "3",
        .vhex_id = VKEY_3,          .vhex_name = "3"
    },
    {
        .sdl2_id = SDLK_4,          .sdl2_name = "4",
        .vhex_id = VKEY_4,          .vhex_name = "4"
    },
    {
        .sdl2_id = SDLK_5,          .sdl2_name = "5",
        .vhex_id = VKEY_5,          .vhex_name = "5"
    },
    {
        .sdl2_id = SDLK_6,          .sdl2_name = "6",
        .vhex_id = VKEY_6,          .vhex_name = "6"
    },
    {
        .sdl2_id = SDLK_7,          .sdl2_name = "7",
        .vhex_id = VKEY_7,          .vhex_name = "7"
    },
    {
        .sdl2_id = SDLK_8,          .sdl2_name = "8",
        .vhex_id = VKEY_8,          .vhex_name = "8"
    },
    {
        .sdl2_id = SDLK_9,          .sdl2_name = "9",
        .vhex_id = VKEY_9,          .vhex_name = "9"
    },

    /* Arrows */
    {
        .sdl2_id = SDLK_RIGHT,      .sdl2_name = "right arrow",
        .vhex_id = VKEY_RIGHT,      .vhex_name = "right arrow"
    },
    {
        .sdl2_id = SDLK_LEFT,       .sdl2_name = "left arrow",
        .vhex_id = VKEY_LEFT,       .vhex_name = "left arrow"
    },
    {
        .sdl2_id = SDLK_DOWN,       .sdl2_name = "down arrow",
        .vhex_id = VKEY_DOWN,       .vhex_name = "down arrow"
    },
    {
        .sdl2_id = SDLK_UP,         .sdl2_name = "up arrow",
        .vhex_id = VKEY_UP,         .vhex_name = "up arrow"
    },

    /* special keys */
    {
        .sdl2_id = SDLK_KP_ENTER,   .sdl2_name = "enter",
        .vhex_id = VKEY_EXE,        .vhex_name = "EXE"
    },
    {
        .sdl2_id = SDLK_LSHIFT,     .sdl2_name = "left shift",
        .vhex_id = VKEY_ALPHA,      .vhex_name = "ALPHA"
    },
    {
        .sdl2_id = SDLK_RSHIFT,     .sdl2_name = "right shift",
        .vhex_id = VKEY_SHIFT,      .vhex_name = "SHIFT"
    },
    {
        .sdl2_id = SDLK_ESCAPE,     .sdl2_name = "echap",
        .vhex_id = VKEY_EXIT,       .vhex_name = "EXIT"
    },
    {
        .sdl2_id = SDLK_BACKSPACE,  .sdl2_name = "backspace",
        .vhex_id = VKEY_DEL,        .vhex_name = "DEL"
    },
    {
        .sdl2_id = SDLK_TAB,        .sdl2_name = "tab",
        .vhex_id = VKEY_MENU,       .vhex_name = "MENU"
    },

    /* end of translation table */
    {
        .sdl2_id = -1, .sdl2_name = NULL,
        .vhex_id = -1, .vhex_name = NULL
    }
};




//---
// keycache primitives
//---

static int sdl_keycache_event_pop(vkey_event_t *event)
{
    SDL_Event evt;
    int ret;

    while (1)
    {
        ret = SDL_PollEvent(&evt);
        if (ret == 0) {
            if (event != NULL) {
                event->key = VKEY_NONE;
                event->type = VKEYEV_NONE;
            }
            return 0;
        }

        if (evt.type == SDL_QUIT)
            exit(0);
        if (evt.type != SDL_KEYDOWN && evt.type != SDL_KEYUP)
            continue;

        for (int i = 0; key_translation[i].sdl2_name != NULL; ++i) {
            if (evt.key.keysym.sym != key_translation[i].sdl2_id)
                continue;

            if (event != NULL) {
                event->time = 0;
                event->key  = key_translation[i].vhex_id;
                event->type = VKEYEV_DOWN;
                if (evt.type == SDL_KEYDOWN)
                    event->type = VKEYEV_UP;
            }
            return 1;
        }
    }
}

//---
// Internal fake driver definition
//---


/* __keysc_configure() : configure the keyboard module */
static void __keysc_configure(void)
{
    printf("[drv] keyboard: keymap notes:\n");
    printf("   +-------------+-------------+\n");
    printf("   |     OS      |     Vhex    |\n");
    printf("   +-------------+-------------+\n");
    for (int i = 0; key_translation[i].sdl2_name != NULL; ++i) {
        printf(
            "   | %-11s | %-11s |\n",
            key_translation[i].sdl2_name,
            key_translation[i].vhex_name
        );
    }
    printf("   +-------------+-------------+\n");
}

/* __keysc_hsave() : save hardware information */
static void __keysc_hsave(void)
{
    // Nothing to do, this is a fake driver
    ;
}

/* __keysc_hrestore() : restore hadware information */
static void __keysc_hrestore(void)
{
    // Nothing to do, this is a fake driver
    ;
}

struct vhex_driver drv_keysc = {
    .name       = "SDL2 Keyboard",
    .hsave      = (void*)&__keysc_hsave,
    .hrestore   = (void*)&__keysc_hrestore,
    .configure  = (void*)&__keysc_configure,
    .state_size = 4,
    .flags = {
        .KEYBOARD = 1,
        .SHARED   = 0,
        .UNUSED   = 0,
    },
    .module_data = &(struct keyboard_drv_interface){
        .keycache_init       = NULL,
        .keycache_keydown    = NULL,
        .keycache_event_wait = NULL,
        .keycache_event_pop  = &sdl_keycache_event_pop,
        .keycache_event_push = NULL,
        .keycache_quit       = NULL,
    }
};
VHEX_DECLARE_DRIVER(03, drv_keysc);
