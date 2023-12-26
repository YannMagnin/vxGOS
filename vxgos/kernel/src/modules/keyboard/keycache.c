#include <vhex/keyboard.h>
#include <vhex/keyboard/interface.h>
#include <vhex/keyboard/keycode.h>
#include <vhex/driver/cpu.h>
#include <vhex/timer.h>
#include <vhex/driver.h>

#include <stdlib.h>
#include <string.h>

struct {
    struct keyboard_drv_interface driver;
} keyboard_info;


//---
// Module init
//---

/* keycache_init() : keycache constructor */
void keycache_init(void)
{
    struct vhex_driver *driver = vhex_driver_table();
    for (int i = 0; i < vhex_driver_count(); ++i) {
        if (driver[i].flags.KEYBOARD) {
            memcpy(
                &keyboard_info.driver,
                driver[i].module_data,
                sizeof(struct keyboard_drv_interface)
            );
            break;
        }
    }
    if (keyboard_info.driver.keycache_init != NULL)
        keyboard_info.driver.keycache_init();
}

/* keycache_init() : keycache descructor */
void keycache_quit(void)
{
    if (keyboard_info.driver.keycache_quit != NULL)
        keyboard_info.driver.keycache_quit();
}

//---
// Public API
//---

/* keycache_keydown() : check the current key state */
int keycache_keydown(vkey_t key)
{
    if (keyboard_info.driver.keycache_keydown != NULL)
        return keyboard_info.driver.keycache_keydown(key);
    return -1;
}

/* keycache_event_wait() : wait until the next event or the timeout != 0 */
int keycache_event_wait(vkey_event_t *event, volatile int *timeout)
{
    if (keyboard_info.driver.keycache_event_wait != NULL)
        return keyboard_info.driver.keycache_event_wait(event, timeout);
    return 0;
}

/* keycache_event_pop() : pop the first key event in the cache */
int keycache_event_pop(vkey_event_t *event)
{
    if (keyboard_info.driver.keycache_event_pop != NULL)
        return keyboard_info.driver.keycache_event_pop(event);
    return 0;
}

/* keycache_event_push() : push a key event in the cache */
int keycache_event_push(vkey_event_t *event)
{
    if (keyboard_info.driver.keycache_event_push != NULL)
        return keyboard_info.driver.keycache_event_push(event);
    return 0;
}
