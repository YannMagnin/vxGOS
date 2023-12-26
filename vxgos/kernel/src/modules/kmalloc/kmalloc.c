//---
// vhex:kmalloc:kmalloc - Main allocator routines
//---

#include <vhex/kmalloc.h>
#include <vhex/defs/utils.h>
#include <vhex/config.h>

#include <string.h>

/* Maximum number of arenas */
#define KMALLOC_ARENA_MAX 8

/* List of arenas in order of consideration */
static kmalloc_arena_t *arenas[KMALLOC_ARENA_MAX] = { 0 };

//---
// Allocation functions
//---

/* kmalloc_get_area() : get kmalloc area using its name */
kmalloc_arena_t *kmalloc_get_arena(char const *name)
{
    for(int i = 0; i < KMALLOC_ARENA_MAX; i++) {
        if(arenas[i] && !strcmp(arenas[i]->name, name))
            return (arenas[i]);
    }
    return (NULL);
}

/* Find the arena that contains a given block */
static kmalloc_arena_t *arena_owning(void *ptr)
{
    for(int i = 0; i < KMALLOC_ARENA_MAX; i++) {
        kmalloc_arena_t *a = arenas[i];
        if(a == NULL) continue;

        if((a->start <= ptr && ptr < a->end) ||
           (a->start == NULL && a->end == NULL))
            return (a);
    }
    return (NULL);
}

/* kmalloc(): Allocate memory in one of the available arenas */
void *kmalloc(size_t size, char const *name)
{
    if(size == 0) return (NULL);

    for(int i = 0; i < KMALLOC_ARENA_MAX; i++)
    {
        /* Check the area validity */
        if(arenas[i] == NULL)
            continue;
        kmalloc_arena_t *a = arenas[i];
        if(name && strcmp(a->name, name)) continue;
        if(!name && !a->is_default) continue;

        /* Try to allocate in this arena */
        void *rc = a->malloc(size, a->data);

        /* Maintain statistics */
        struct kmalloc_stats *s = &a->stats;
        if(rc != NULL) {
            s->live_blocks++;
            s->peak_live_blocks = max(
                s->live_blocks,
                s->peak_live_blocks
            );
            s->total_volume += size;
            s->total_blocks++;
            return (rc);
        } else {
            s->total_failures++;
        }
    }

    return (NULL);
}

/* krealloc(): Reallocate memory */
void *krealloc(void *ptr, size_t size)
{
    if(ptr == NULL)
        return kmalloc(size, NULL);
    if(size == 0) {
        kfree(ptr);
        return NULL;
    }

    kmalloc_arena_t *a = arena_owning(ptr);
    if(a == NULL)
        return NULL;

    void *rc = a->realloc(ptr, size, a->data);

    /* Maintain statistics */
    if(rc != NULL) {
        a->stats.total_volume += size;
        a->stats.total_blocks++;
    } else {
        a->stats.total_failures++;
    }

    return (rc);
}

/* kfree(): Free memory allocated with kalloc() */
void kfree(void *ptr)
{
    if(ptr == NULL) return;

    /* If this condition fails, then the pointer is invalid */
    kmalloc_arena_t *a = arena_owning(ptr);
    if(a == NULL)
        return;

    a->free(ptr, a->data);

    /* Maintain statistics */
    a->stats.live_blocks--;
}

/* kmalloc_add_arena(): Add a new arena to the heap source */
bool kmalloc_add_arena(kmalloc_arena_t *arena)
{
    for(int i = 0; i < KMALLOC_ARENA_MAX; i++) {
        if(arenas[i] == NULL) {
            arenas[i] = arena;
            return (true);
        }
    }
    return (false);
}

//---
// Module declaraction
//---

#include <vhex/module.h>

/* __kmalloc_init() : initialize the kmalloc module */
static void __kmalloc_init(void)
{
    /* NOTE: this part SHOULD be provided by the board definition */
    extern void kmalloc_init(void);
    kmalloc_init();
}

/* __kmalloc_quit() : quit the kmalloc module */
static void __kmalloc_quit(void)
{
    /* NOTE: this part SHOULD be provided by the board definition */
    extern void kmalloc_quit(void);
    kmalloc_quit();
}

/* declare the module */

struct vhex_module mod_kmalloc = {
    .name = "kmalloc",
    .init = &__kmalloc_init,
    .quit = &__kmalloc_quit,
};
VHEX_DECLARE_MODULE(01, mod_kmalloc);
