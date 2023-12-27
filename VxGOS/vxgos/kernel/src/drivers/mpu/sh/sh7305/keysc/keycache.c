#include <vhex/driver/mpu/sh/sh7305/keysc.h>
#include <vhex/driver/mpu/sh/sh7305/tmu.h>
#include <vhex/keyboard.h>
#include <vhex/timer.h>
#include <vhex/driver/cpu.h>

#include <stdlib.h>
#include <string.h>

struct {
    vkey_event_t keycache[96];
    struct {
        vkey_event_t *list;
        int nb_evt_max;
        int idx;
    } queue;
    struct {
        tid_t id;
        volatile uint16_t counter;
    } timer;
} keyinfo;

//---
// kernel-level API
//---

static int __sh7305_keycache_keyframe(volatile uint16_t *counter)
{
    *counter += 1;
    return TIMER_CONTINUE;
}

/* sh7305_keycache_init() : initialise the keycache information */
int sh7305_keycache_init(void)
{
    for (int i = 0; i < 96 ; ++i) {
        keyinfo.keycache[i].rep_type = VKEYEV_REP_NONE;
        keyinfo.keycache[i].type = VKEYEV_UP;
        keyinfo.keycache[i].key = i;
    }

    keyinfo.queue.idx = -1;
    keyinfo.queue.nb_evt_max = 16;
    keyinfo.queue.list = calloc(
        keyinfo.queue.nb_evt_max,
        sizeof(vkey_event_t)
    );

    keyinfo.timer.id = sh7305_tmu_configure(
        7215,
        VHEX_CALL(
            &__sh7305_keycache_keyframe,
            &keyinfo.timer.counter
        )
    );
    sh7305_tmu_start(keyinfo.timer.id);
    return 0;
}

/* sh7305_keycache_quit() : quit the keycache */
int sh7305_keycache_quit(void)
{
    sh7305_tmu_stop(keyinfo.timer.id);
    free(keyinfo.queue.list);
    keyinfo.queue.idx = -1;
    keyinfo.queue.nb_evt_max = 0;
    return 0;
}


//---
// driver-level API
//---

/* sh7305_keycache_keydown() : return the key status */
int sh7305_keycache_keydown(vkey_t key)
{
    int info;

    cpu_atomic_start();
    info = keyinfo.keycache[key].type;
    cpu_atomic_end();

    return info == VKEYEV_DOWN;
}


/* sh7305_keycache_event_push() : push event on the keycache */
int sh7305_keycache_event_push(vkey_event_t *event)
{
    if (event == NULL)
        return -1;

    cpu_atomic_start();

    keyinfo.queue.idx += 1;
    if (keyinfo.queue.idx >= keyinfo.queue.nb_evt_max) {
        keyinfo.queue.nb_evt_max += keyinfo.queue.nb_evt_max;
        keyinfo.queue.list = reallocarray(
            keyinfo.queue.list,
            keyinfo.queue.nb_evt_max,
            sizeof(vkey_event_t)
        );
    }

    keyinfo.queue.list[keyinfo.queue.idx].time = keyinfo.timer.counter;
    keyinfo.queue.list[keyinfo.queue.idx].type = event->type;
    keyinfo.queue.list[keyinfo.queue.idx].key  = event->key;

    cpu_atomic_end();
    return 0;
}

/* sh7305_keycache_event_push() : pop event from the keycache */
int sh7305_keycache_event_pop(vkey_event_t *event)
{
    int ret;

    cpu_atomic_start();

    if (keyinfo.queue.idx < 0) {
        if (event != NULL) {
            event->key  = VKEY_NONE;
            event->type = VKEYEV_NONE;
        }
        ret = 0;
    } else {
        if (event != NULL) {
            event->time = keyinfo.queue.list[0].time;
            event->type = keyinfo.queue.list[0].type;
            event->key  = keyinfo.queue.list[0].key;
        }

        memcpy(
            &keyinfo.queue.list[0],
            &keyinfo.queue.list[1],
            sizeof(vkey_event_t) * keyinfo.queue.idx
        );

        keyinfo.queue.idx -= 1;
        ret = 1;
    }

    cpu_atomic_end();
    return ret;
}

/* sh7305_keycache_event_wait() : wait event or timeout != 0 */
int sh7305_keycache_event_wait(vkey_event_t *event, volatile int *timeout)
{
    do {
        if (sh7305_keycache_event_pop(event))
            return 1;
        if (timeout != NULL && timeout != 0) {
            if (event != NULL) {
                event->type = VKEYEV_NONE;
                event->key  = VKEY_NONE;
            }
            return 0;
        }
        __asm__("sleep");
    } while (1);
}

/* sh7305_keycache_update() : involved by the interrupt handler

   Note : this function SHOULD be involved with atomic context !! */
void sh7305_keycache_update(vkey_t key, int status)
{
    status = (status != 0) ? VKEYEV_DOWN : VKEYEV_UP;

    if (keyinfo.keycache[key].type == status)
        return;

    keyinfo.keycache[key].type = status;
    sh7305_keycache_event_push(&keyinfo.keycache[key]);
}
