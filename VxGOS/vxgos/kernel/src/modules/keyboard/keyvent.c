#include <vhex/keyboard/keyvent.h>
#include <vhex/keyboard/keycache.h>

/* keyvent_poll(): Poll the next keyboard event */
int keyvent_poll(vkey_event_t *evt)
{
    return keycache_event_pop(evt);
}

/* waitevent(): Wait for the next keyboard event */
int keyvent_wait(vkey_event_t *evt, volatile int *timeout)
{
    return keycache_event_wait(evt, timeout);
}

/* clearevents(): Clear all events waiting in the queue */
void keyvent_clear(void)
{
    vkey_event_t evt;

    while (keyvent_poll(&evt)) {
        ;
    }
}
