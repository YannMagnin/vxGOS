#include <stdbool.h>

void debug_main(void)
{
    dupdate();
    while (true) {
        __asm__ volatile ("sleep");
    }
}
