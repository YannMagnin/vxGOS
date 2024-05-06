#ifndef __VHEX_TIMER_TYPES__
# define __VHEX_TIMER_TYPES__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>

/* timer ID */
typedef int tid_t;

/* Context object, has an elasped delay and a recursion level. This object can
   be created on the stack of a function that measures its execution time,
   except if the function is recursive, in which case it should be either
   static or global. */
typedef struct timer_prof
{
    uint32_t rec;
    uint32_t elapsed;
    uint32_t anchor;
} VPACKED(4) timer_prof_t;

#endif /* __VHEX_TIMER_TYPES__ */
