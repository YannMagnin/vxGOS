
//---
// Public
//---

/* console_key_get() : small one-shot key waiter */
int console_key_get(void)
{
    while(1) {
        __asm__ volatile ("nop");
    }
    return -1;
}
