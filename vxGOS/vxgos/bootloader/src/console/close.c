#include <stdlib.h>
#include <string.h>

#include "bootloader/console.h"

//---
// Public
//---

/* console object */
extern struct console console;

/* console_close(): Uninitialize the terminal */
int console_close(void)
{
    if (console.output.buffer.data != NULL)
        free(console.output.buffer.data);
    memset(&console, 0x00, sizeof(struct console));
    return 0;
}
