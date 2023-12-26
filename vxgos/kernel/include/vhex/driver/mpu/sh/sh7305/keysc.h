#ifndef __VHEX_ARCH_SH7305_KEYSC__
# define __VHEX_ARCH_SH7305_KEYSC__

#include <vhex/defs/types.h>
#include <vhex/defs/attributes.h>

struct __sh7305_keysc_s
{
    const uint16_t KIUDATA[6];

    word_union(KIUCNTREG,
        uint16_t KEYIE          :1;    /* Key Interrupt enable */
        uint16_t                :13;
        uint16_t _0             :1;    /* Unknown (TODO) */
        uint16_t _1             :1;    /* Unknown (TODO) */
    );

    word_union(KIAUTOFIXREG,
        uint16_t KEYBE          :1;    /* enable AUTOFIX (?) */
        uint16_t                :8;
        uint16_t _0             :3;    /* Unknown (TODO) */
        uint16_t                :1;
        uint16_t _1             :3;    /* Unknown (TODO) */
    );

    word_union(KIUMODEREG,
        uint16_t                :4;
        uint16_t _0             :1;    /* Unknown (TODO) */
        uint16_t _1             :1;    /* Unknown (TODO) */
        uint16_t _2             :1;    /* Unknown (TODO) */
        uint16_t                :9;
    );

    const word_union(KIUSTATEREG,
        uint16_t                :15;
        uint16_t STATE          :1;
    );

    word_union(KIUINTREG,
        uint16_t                :1;    /* All 0 */
        uint16_t KYCPU_IE       :7;    /* KEYSC interrupt setup */
        uint16_t const STATUS   :8;    /* "type" of interruption */
    );

    word_union(KIUWSETREG,
        uint16_t                :1;
        uint16_t TIME           :15;    /* Time between interrupt */
    );

    uint16_t KIUINTERVALREG;

    word_union(KYOUTDR,
        uint16_t                :4;
        uint16_t KYO5DT         :2;    /* Key Controll ouput 5 */
        uint16_t KYO4DT         :2;    /* Key Controll ouput 4 */
        uint16_t KYO3DT         :2;    /* Key Controll ouput 3 */
        uint16_t KYO2DT         :2;    /* Key Controll ouput 2 */
        uint16_t KYO1DT         :2;    /* Key Controll ouput 1 */
        uint16_t KYO0DT         :2;    /* Key Controll ouput 0 */
    );

    word_union(KYINDR,
        uint16_t                :9;
        uint16_t KYDIR6         :1;    /* Key Direction 6 */
        uint16_t KYDIR5         :1;    /* Key Direction 5 */
        uint16_t KYDIR4         :1;    /* Key Direction 4 */
        uint16_t KYDIR3         :1;    /* Key Direction 3 */
        uint16_t KYDIR2         :1;    /* Key Direction 2 */
        uint16_t KYDIR1         :1;    /* Key Direction 1 */
        uint16_t KYDIR0         :1;    /* Key Direction 0 */
    );
} VPACKED(2);

#define SH7305_KEYSC    (*(volatile struct __sh7305_keysc_s *)0xa44b0000)

//---
// Internal API
//---

#include <vhex/keyboard/types.h>

/* sh7305_keycache_init() : initialise the keycache information */
extern int sh7305_keycache_init(void);

/* sh7305_keycache_add_key() : involved by the interrupt handler */
extern void sh7305_keycache_update(vkey_t key, int status);

/* sh7305_keycache_quit() : quit the keycache */
extern int sh7305_keycache_quit(void);

/* sh7305_keycache_keydown() : return the key status */
extern int sh7305_keycache_keydown(vkey_t key);

/* sh7305_keycache_event_wait() : wait event or timeout != 0 */
extern int sh7305_keycache_event_wait(vkey_event_t *e, volatile int *timeout);

/* sh7305_keycache_event_push() : pop event from the keycache */
extern int sh7305_keycache_event_pop(vkey_event_t *event);

/* sh7305_keycache_event_push() : push event on the keycache */
extern int sh7305_keycache_event_push(vkey_event_t *event);

#endif /* __VHEX_ARCH_SH7305_KEYSC__ */
