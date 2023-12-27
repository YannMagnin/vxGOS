#include "vhex/display.h"
#include "vhex/display/stack.h"

/* dupdate(): Push the video RAM to the display driver */
void dupdate(void)
{
    dstack_render();
}
